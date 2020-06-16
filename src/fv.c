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

#include "fv.h"
#include "draw.h"
#include "input.h"
#include "fv_file.h"

/* prototypes */
static void parse_args();
static void init_fv();
static void get_window_size();
static void enable_raw_mode();
static void disable_raw_mode();
static void quit();

/* global fv struct */
static struct fv config;

int main(int argc, char *argv[])
{
    parse_args();
    init_fv();
    enable_raw_mode();
    clear_screen();
    while(1) {
        refresh_screen(&config);
        process_input(&config);
    }
    return EXIT_SUCCESS;
}

/* Parses runtime arguments */
static void parse_args(int argc, char *argv[])
{
    if (argc == 1){
        printf("A filename argument is required.\n");
        exit(EXIT_FAILURE);
    }
    config.filename = argv[1];
}

/* Initialises the config struct */
static void init_fv()
{
    /* register disable_raw_mode() to be called at exit */
    if (atexit(quit) != 0)
        DIE("Failed to register exit function");
    /* obtain window size */
    get_window_size(&config.trows, &config.tcols);
    /* read file contents */
    config.f = handle_file(config.filename);
    /* set vertical and horizontal offsets to zero */
    config.voffset = 0;
    config.hoffset = 0;
    /* initial prompt */
    config.prompt = malloc(config.tcols);
    memset(config.prompt, '\0', config.tcols);
    config.prompt_idx = 0;
}

/* returns the number of columns and rows in the terminal window
 * via *rows and *cols pointers. It uses ioctl() syscall to obtain
 * this information. It it fails, DIE() macro is called
 */
static void get_window_size(int *rows, int *cols)
{
    struct winsize ws;
    if (ioctl(1, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
        DIE("Failed to obtain window size.");
    *rows = ws.ws_row;
    *cols = ws.ws_col;
}

/* stores original termios struct and switches terminal raw mode.
 * If any syscall fails, this function calls the DIE macro
 */
static void enable_raw_mode()
{
    /* switch to alternate screen buffer */
    write(STDOUT_FILENO, "\x1b[?1049h", 8);

    if (tcgetattr(STDIN_FILENO, &(config.orig)) == -1)
        DIE("Failed to obtain terminal attributes.");
    struct termios raw = config.orig;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* raw.c_lflag &= ~(ECHO | ICANON | IEXTEN); */
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        DIE("Failed to switch to raw mode");

}

/* switches the terminal to whatever mode it was in before the start
 * of the program. This function is automatically called during normal
 * exit (this includes DIE()).
 */
static void disable_raw_mode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &config.orig) == -1)
        DIE("Failed to switch back to canonical mode.");
    /* switch back to original screen buffer */
    write(STDOUT_FILENO, "\x1b[?1049l", 8);
}

/* cleanup memory and exit */
static void quit()
{
    /* restore terminal */
    clear_screen();
    disable_raw_mode();
    /* free file memory. If a relloc() error occured in read_file(), already
     * allocated memory is freed and exit() is called. In that case, config.f
     * is already NULL so don't free it again !*/
    if (config.f == NULL)
        return ;
    int i = 0;
    for(i = 0; i < config.f->line_count; i++) {
        free(config.f->contents[i]->line);
        free(config.f->contents[i]);
    }
    return ;
}
