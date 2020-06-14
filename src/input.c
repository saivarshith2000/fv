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

/* adjusts cfg->voffset to scroll up n lines */
static void scroll_up(struct fv *cfg, int n)
{
    if (cfg->voffset - n < 0)
        cfg->voffset = 0;
    else
        cfg->voffset -= n;
}

/* adjusts cfg->voffset to scroll down n lines */
static void scroll_down(struct fv *cfg, int n)
{
    int max_voffset = cfg->f->line_count - cfg->trows + 3;
    if (cfg->voffset + n <= max_voffset)
        cfg->voffset += n;
    else
        cfg->voffset = max_voffset;
}

/* Obtains user input via read_key() and modifies the state of fv struct */
void process_input(struct fv *config)
{
    int key = read_key();

    /* handle numeric input */
    if (key >= '0' && key <= '9') {
        int num = key - 48;
        while((key = read_key())){
            if (key >= '0' && key <= '9') {
                num *= 10;
                num += key - 48;
            } else if (key == '\r' || key == '\n') {
                /* jump to line number num */
                config->voffset = num > 1 ? num - 1 : num;
                break;
            } else if (key == 'j') {
                /* scrolling down num lines */
                scroll_down(config, num);
                break;
            } else if (key == 'k') {
                /* scrolling up num lines */
                scroll_up(config, num);
                break;
            } else {
                /* invalid input */
                break;
            }
        }
        return ;
    }

    struct fv_file *f = config->f;
    switch(key) {
    case 'j':
        /* scroll down 1 line */
        scroll_down(config, 1);
        break;

    case 'k':
        /* scroll up 1 line */
        scroll_up(config, 1);
        break;

    case 'g':
        /* scroll to top */
        config->voffset = 0;
        break;

    case 'G':
        /* scroll to bottom */
        if (f->line_count - config->voffset > config->trows + 3)
            config->voffset = f->line_count - config->trows + 3;
        break;

    case 'q':
        /* quit */
        exit(EXIT_SUCCESS);
    }
    return ;
}
