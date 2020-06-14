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
#include <string.h>

#include "draw.h"
#include "dynbuf.h"

/* clear screen */
void clear_screen()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
}

/* draws a status bar at the bottom of the screen */
static void status_bar(struct fv *cfg, struct dynbuf *dyn)
{
    /* place cursor at the bottom row of the screen and invert colors */
    char temp[16];
    sprintf(temp, "\x1b[%d;0H\x1b[7m", cfg->trows-1);
    dynbuf_insert(dyn, temp, strlen(temp));
    char status[cfg->tcols + 16];
    sprintf(status, " viewing: %s [%d/%d]", cfg->f->filename, cfg->voffset, cfg->f->line_count);
    dynbuf_insert(dyn, status, strlen(status));
    /* fill out bar with spaces */
    int i = 0;
    int space = cfg->tcols - strlen(status);
    while(i++ < space)
        dynbuf_insert(dyn, " ", 1);
    dynbuf_insert(dyn, "\x1b[27m", 5);
}

/* refreshes terminal screen. This function is called on every valid input */
void refresh_screen(struct fv *cfg)
{
    struct dynbuf dyn = DYNBUF_INIT;
    dynbuf_insert(&dyn, "\x1b[?25l", 6);
    dynbuf_insert(&dyn, "\x1b[H", 3);
    /* 2 rows are reserved for status bar and prompt */
    int trows = cfg->trows - 3;
    struct filerow **contents = cfg->f->contents;
    int line_count_digs = cfg->f->line_count_digs;
    int line_count = cfg->f->line_count;
    int i = cfg->voffset;
    int lines_drawn = 0;
    /* draw lines till are all lines are drawn or rows are unavailable */
    while(lines_drawn < trows && i < line_count) {
        /* clear line */
        dynbuf_insert(&dyn, "\x1b[K", 3);
        /* draw line number */
        char num[line_count_digs + 3];
        sprintf(num, "%*d. | ", line_count_digs, i + 1);
        dynbuf_insert(&dyn, num, strlen(num));
        /* draw line */
        dynbuf_insert(&dyn, contents[i]->line, contents[i]->len);
        /* draw newline and carriage return */
        dynbuf_insert(&dyn, "\r\n", 2);
        i++;
        lines_drawn++;
    }
    /* draw status bar */
    status_bar(cfg, &dyn);
    dynbuf_insert(&dyn, "\x1b[H", 3);
    dynbuf_insert(&dyn, "\x1b[?25h", 6);
    write(STDOUT_FILENO, dyn.buf, dyn.ptr);
    return ;
}
