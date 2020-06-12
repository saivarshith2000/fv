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

#include <unistd.h>
#include <errno.h>

#include "input.h"

/* Waits for user to enter a key and returns it. It handles read timeout
 * and errors. If read() error occurs, it will DIE()
 */
static int read_key()
{
    char c;
    int br;
    while((br = read(STDIN_FILENO, &c, 1)) != 1) {
        if (br == -1 && errno != EAGAIN) {
            /* If any error other than timeout occurs, DIE() */
            DIE("read() failed");
        }
    }
    return c;
}

/* Obtains user input via read_key() and modifies the state of fv struct */
int process_input(struct fv *config)
{
    int key = read_key();
    switch(key) {
        case 'j':
            {
                /* scroll down 1 line */
                if (config->f->line_count - config->voffset > config->trows)
                    config->voffset++;
                break;
            }
        case 'k':
            {
                /* scroll up 1 line */
                if (config->voffset != 0)
                    config->voffset--;
                break;
            }
        case 'g':
            {
                /* scroll to top */
                config->voffset = 0;
                break;
            }
        case 'G':
            {
                /* scroll to bottom */
                config->voffset = config->f->line_count - config->trows;
                break;
            }
        case 'q':
            {
                /* quit */
                return -1;
            }
    }
    return 0;
}
