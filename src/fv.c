/*
    fv: A simple terminal based file viewer.
    Copyright (C) 2020  Sai Varshith

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Author Email: hosvarshith@gmail.com
*/

#include "fv.h"
#include "file.h"

/* the fv struct contains all the terminal and file variables in one place */
struct fv {
    struct termios orig;     /* termios struct before going into raw mode */
    int trows, tcols;        /* rows and columns of the terminal screen */
    char *filename;          /* name of the file open with fv */
    struct file *f;          /* pointer to the file struct. see src/file.h */
};

/* prototypes */
static void parse_args();
static void init_fv();
static void get_window_size();
static void enable_raw_mode();
static void disable_raw_mode();
static void refresh_screen();
static void process_input();

/* global fv struct */
static struct fv config;

int main(int argc, char *argv[])
{
    parse_args();
    init_fv();
    enable_raw_mode();
    while(1) {
        refresh_screen();
        process_input();
    }
    return EXIT_SUCCESS;
}

/* prints the contents of file */
static void refresh_screen()
{
    /* set cursor to upper left corner */
    write(STDOUT_FILENO, "\x1b[H", 3);
}

/* process user input */
static void process_input()
{

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
    /* obtain window size */
    get_window_size(&config.trows, &config.tcols);
    /* read file contents */
    config.f =  handle_file(config.filename);

    /* testing lol */
    struct filerow *cursor = config.f->contents;
    while(cursor != NULL) {
        printf("%s", cursor->line);
        cursor = cursor->next;
    }
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
    if (tcgetattr(STDIN_FILENO, &(config.orig)) == -1)
        DIE("Failed to obtain terminal attributes.");
    /* register disable_raw_mode() to be called at exit */
    if (atexit(disable_raw_mode) != 0)
        DIE("Failed to register exit function");
    struct termios raw = config.orig;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        DIE("Failed to switch to raw mode");
}

/* switches the terminal to whatever mode it was in before the start
 * of the program. This function is automatically called during normal
 * exit (this includes DIE()).
 */
static void disable_raw_mode(void *arg)
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &config.orig) == -1)
        DIE("Failed to switch back to canonical mode.");
}
