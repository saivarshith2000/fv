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

/* moves cursor to (row, col) */
static void move_cursor(int row, int col)
{
    char temp[32];
    sprintf(temp, "\x1b[%d;%dH", row, col);
    write(STDOUT_FILENO, temp, strlen(temp));
}

/* draws a status bar at the bottom of the screen */
static void status_bar(struct fv *cfg)
{
    struct dynbuf dyn = DYNBUF_INIT;
    /* invert colors */
    dynbuf_insert(&dyn, "\x1b[7m", 4);
    char status[cfg->tcols + 16];
    sprintf(status, " File: %s [%d/%d]", cfg->f->filename, cfg->voffset, cfg->f->line_count);
    dynbuf_insert(&dyn, status, strlen(status));
    /* fill out bar with spaces */
    int i = 0;
    int space = cfg->tcols - strlen(status);
    while(i++ < space)
        dynbuf_insert(&dyn, " ", 1);
    dynbuf_insert(&dyn, "\x1b[27m\r\n", 7);
    write(STDOUT_FILENO, dyn.buf, dyn.ptr);
    dynbuf_free(&dyn);
}

/* draws the prompt below status bar */
static void prompt(struct fv *cfg)
{
    if (cfg->prompt_idx)
        write(STDOUT_FILENO, cfg->prompt, cfg->prompt_idx);
    else
        write(STDOUT_FILENO, "\x1b[K", 3);
    return ;
}

/* draw rows */
void draw_rows(struct fv *cfg)
{
    struct dynbuf dyn = DYNBUF_INIT;
    /* last 3 rows are for - padding, status bar, prompt */
    int rows = cfg->trows - 3;
    struct filerow **contents = cfg->f->contents;
    int linenum_padding = cfg->f->line_count_digs;
    int line_count = cfg->f->line_count;
    int lines_drawn = 0;
    int i = cfg->voffset;
    while(lines_drawn < rows && i < line_count) {
        /* clear line */
        dynbuf_insert(&dyn, "\x1b[K", 3);
        /* draw line number */
        char num[linenum_padding + 3];
        sprintf(num, "%*d| ", linenum_padding, i + 1);
        dynbuf_insert(&dyn, num, strlen(num));
        /* draw line */
        dynbuf_insert(&dyn, contents[i]->line, contents[i]->len);
        /* draw newline and carriage return */
        dynbuf_insert(&dyn, "\r\n", 2);
        i++;
        lines_drawn++;
    }
    write(STDIN_FILENO, dyn.buf, dyn.ptr);
    dynbuf_free(&dyn);
}

/* refreshes terminal screen. This function is called on every valid input */
void refresh_screen(struct fv *cfg)
{
    /* hide cursor */
    write(STDOUT_FILENO, "\x1b[?25h", 6);
    /* move cursor to top left */
    move_cursor(0, 0);
    draw_rows(cfg);
    /* place cursor at the (bottom - 1) row of the screen */
    move_cursor(cfg->trows-1, 0);
    status_bar(cfg);
    prompt(cfg);
    /* move cursor to prompt */
    move_cursor(cfg->trows, cfg->prompt_idx+1);
    /* unhide cursor */
    write(STDOUT_FILENO, "\x1b[?25h", 6);
    return ;
}
