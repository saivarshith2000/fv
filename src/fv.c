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
#include <sys/ioctl.h>

#include "fv.h"
#include "draw.h"
#include "input.h"

/* prototypes */
static void parse_args(fv_state *state, int argc, char *argv[]);
static void init_fv(fv_state *state);
static int get_window_size(int *rows, int *cols);
static void enable_raw_mode(fv_state *state);
static int disable_raw_mode(fv_state *state);

int main(int argc, char *argv[])
{
    /* initialize fv */
    fv_state state = {0};
    parse_args(&state, argc, argv);
    init_fv(&state);
    enable_raw_mode(&state);
    clear_screen();
    while(1) {
        refresh_screen(&state);
        process_input(&state);
    }
    return EXIT_SUCCESS;
}

/* Parses runtime arguments */
static void parse_args(fv_state *state, int argc, char *argv[])
{
    if (argc == 1){
        printf("A filename is required.\n");
        exit(EXIT_FAILURE);
    }
    state->filename = argv[1];
}

/* Initialises the config struct */
static void init_fv(fv_state *state)
{
    /* obtain window size */
    if(get_window_size(&state->trows, &state->tcols) == -1)
        quit(state, "Failed to obtain window size", EXIT_FAILURE, 0);
    /* read file contents */
    if (handle_file(state->filename, &state->f) == -1)
        quit(state, "", EXIT_FAILURE, 1);
    /* initial prompt */
    state->prompt = malloc(state->tcols);
    memset(state->prompt, '\0', state->tcols);
}

/* Obtains terminal size using ioctl(). Returns 0 on success and
 * -1 on failure. On success, the size is returned via pointers */
static int get_window_size(int *rows, int *cols)
{
    struct winsize ws;
    if (ioctl(1, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
        return -1;
    *rows = ws.ws_row;
    *cols = ws.ws_col;
    return 0;
}

/* stores original termios struct and switches terminal raw mode.
 * If any syscall fails, this function calls the DIE macro
 */
static void enable_raw_mode(fv_state *state)
{
    /* switch to alternate screen buffer */
    write(STDOUT_FILENO, "\x1b[?1049h", 8);

    if (tcgetattr(STDIN_FILENO, &(state->orig)) == -1)
        quit(state, "Failed to obtain terminal attributes.", EXIT_FAILURE, 0);
    struct termios raw = state->orig;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* raw.c_lflag &= ~(ECHO | ICANON | IEXTEN); */
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        quit(state, "Failed to switch to raw mode", EXIT_FAILURE, 0);
}

/* disable raw mode and switch back to original screen buffer. Returns 0 on
 * success and -1 on failure */
static int disable_raw_mode(fv_state *state)
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &state->orig) == -1)
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
    if (state->f.contents == NULL)
        return ;
    int i = 0;
    for(i = 0; i < state->f.line_count; i++) {
        free(state->f.contents[i]->line);
        free(state->f.contents[i]);
    }
    /* restore terminal */
    if (switch_back) {
        clear_screen();
        disable_raw_mode(state);

    }
    /* msg is not NULL (an error occured). Print the error and exit */
    if (msg) {
        fprintf(stderr, "ERROR: %s\n", msg);
        fprintf(stderr, "ERRNO: %d (%s)", errno, strerror(errno));
    }
    exit(exit_code);
}
