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

/* prototypes */
static void init_fv();
static void enable_raw_mode();
static void disable_raw_mode();

/* global fv struct */
static struct fv config;

int main()
{
    init_fv();
    enable_raw_mode();
    while(1) {

    }
    return EXIT_SUCCESS;
}

/* Initialises the config struct */
static void init_fv()
{
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
