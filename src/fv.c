/*
   MIT License

   Copyright (c) 2020 Sai Varshith

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "fv.h"
#include "draw.h"
#include "input.h"

#define VERSION_STR "fv 1.0.0\n"\
                    "Copyright (c) 2020 Sai Varshith\n"\
                    "fv is licensed under the MIT License\n"\
                    "See https://github.com/saivarshith2000/fv\n"

/* fv state */
static fv_state state = {0};

/* prototypes */
static void parse_args(int argc, char *argv[]);
static void init_fv();
static void get_window_size();
static void prepare_terminal();
static int restore_terminal();

int main(int argc, char *argv[])
{
    /* initialize fv */
    parse_args(argc, argv);
    init_fv(&state);
    prepare_terminal(&state);
    clear_screen();
    while(1) {
        refresh_screen(&state);
        process_input(&state);
    }
    return EXIT_SUCCESS;
}

/* Parses runtime arguments */
static void parse_args(int argc, char *argv[])
{
    int opt;
    while((opt = getopt(argc, argv, "lhv")) != -1) {
        switch(opt) {
            case 'l':
                /* disable line numbering */
                state.disable_linenum = 1;
                break;

            case 'h':
                /* print help string and exit */
                printf("Usage: fv <filename>[:line-number] [-l] [-h] [-v]\n");
                quit(&state, NULL, EXIT_SUCCESS, 0);

            case 'v':
                /* print version and copyright information */
                printf("%s", VERSION_STR);
                quit(&state, NULL, EXIT_SUCCESS, 0);

            default:
                quit(&state, NULL, EXIT_SUCCESS, 0);
        }
    }
    if (optind < argc) {
        char *seperator = strchr(argv[optind], ':');
        if (seperator == NULL) {
            /* if line number is not passed */
            state.filename = argv[optind];
        } else {
            int linenum = strtol(seperator + 1, NULL, 10);
            *seperator = '\0';
            state.filename = argv[optind];
            /* if an error occured during conversion or line number
             * is invalid just open the file silently */
            if (linenum < 1 || errno == ERANGE)
                return ;
            /* this value is later adjusted in init_fv() */
            state.voffset = linenum - 1;
        }
    } else {
        printf("A filename is required. See fv -h for usage\n");
        exit(EXIT_FAILURE);
    }
}

/* CTRL+C handler */
static void ctrlc_handler(){
    quit(&state, NULL, EXIT_SUCCESS, 1);
}

/* Initialises the config struct */
static void init_fv()
{
    /* register signal handlers */
    signal(SIGWINCH, get_window_size);
    signal(SIGINT, ctrlc_handler);

    /* obtain window size */
    get_window_size();
    /* read file contents */
    if (handle_file(state.filename, &state.f) == -1)
        quit(&state, "", EXIT_FAILURE, 1);
    /* check if voffset given by user to valid */
    if (state.voffset > state.f.line_count)
        state.voffset = 0;
    /* initial prompt */
    state.prompt = malloc(state.tcols);
    memset(state.prompt, '\0', state.tcols);
}

/* Obtains terminal size using ioctl(). Returns 0 on success and
 * -1 on failure. On success, the size is returned via pointers */
static void get_window_size()
{
    struct winsize ws;
    if (ioctl(1, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
        quit(&state, "Failed to obtain window size", EXIT_FAILURE, 0);
    state.trows = ws.ws_row;
    state.tcols = ws.ws_col;
}

/* stores original termios struct and switches terminal raw mode.
 * If any syscall fails, this function exits using quit()
 */
static void prepare_terminal()
{

    /* switch to alternate screen buffer */
    write(STDOUT_FILENO, "\x1b[?1049h", 8);

    if (tcgetattr(STDIN_FILENO, &state.orig) == -1)
        quit(&state, "Failed to obtain terminal attributes.", EXIT_FAILURE, 0);
    struct termios raw = state.orig;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        quit(&state, "Failed to switch to raw mode", EXIT_FAILURE, 0);
}

/* disable raw mode and switch back to original screen buffer. Returns 0 on
 * success and -1 on failure */
static int restore_terminal()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &state.orig) == -1)
        return -1;
    /* switch back to original screen buffer */
    write(STDOUT_FILENO, "\x1b[?1049l", 8);
    return 0;
}

/* This function is called when an error occurs or the user press q
 * @state -> pointer to fv_state.
 * @msg -> error message. NULL for normal exit.
 * @exit_code -> exit code. EXIT_FAILURE if error else EXIT_SUCCESS.
 * @switch_back -> if switch_back is 1, terminal switches to raw_mode
 */
void quit(fv_state *state, char *msg, int exit_code, int switch_back)
{
    if (state->f.contents != NULL) {
        int i = 0;
        for(i = 0; i < state->f.line_count; i++) {
            free(state->f.contents[i]->line);
            free(state->f.contents[i]);
        }
    }

    /* restore terminal */
    if (switch_back) {
        clear_screen();
        restore_terminal();
    }

    /* msg is not NULL (an error occured). Print the error */
    if (msg) {
        fprintf(stderr, "ERROR: %s\n", msg);
        fprintf(stderr, "ERRNO: %d (%s)", errno, strerror(errno));
    }
    exit(exit_code);
}
