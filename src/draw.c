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

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "draw.h"

/* Dynamic character buffer data structure. Multiple write()s cause a flickering effect. To avoid
 * this all the data to be written is stored in a dynamic buffer and written in one go */
struct dynbuf {
    char *buf;   /* pointer to buffer */
    int ptr;     /* pointer to the first empty space in buffer */
    int size;    /* length of buffer */
};
#define DYNBUF_CHUNK_SIZE 1024

/* simple macro to init a dynbuf */
#define DYNBUF_INIT {malloc(DYNBUF_CHUNK_SIZE), 0, DYNBUF_CHUNK_SIZE}

/* inserts str into dynbuf. Returns 0 on success and -1 otherwise */
int dynbuf_insert(struct dynbuf *dyn, const char *str, int len)
{
    if (dyn == NULL || dyn->buf == NULL)
        return -1;
    if (dyn->size - dyn->ptr < len) {
        /* realloc() a new chunk if buffer runs out of memory */
        char *newmem = realloc(dyn->buf, dyn->size + DYNBUF_CHUNK_SIZE);
        if (newmem == NULL) {
            /* realloc failed */
            free(dyn->buf);
            free(dyn);
            exit(EXIT_FAILURE);
        }
        dyn->buf = newmem;
        dyn->size += DYNBUF_CHUNK_SIZE;
    }
    memcpy(dyn->buf + dyn->ptr, str, len);
    dyn->ptr += len;
    return 0;
}

void dynbuf_free(struct dynbuf *dyn)
{
    free(dyn->buf);
}
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
static void status_bar(fv_state *state)
{
    struct dynbuf dyn = DYNBUF_INIT;
    /* invert colors */
    dynbuf_insert(&dyn, "\x1b[7m", 4);
    char status[state->tcols + 16];
    sprintf(status, " File: %s [%d/%d]", state->f.filename, state->voffset, state->f.line_count);
    dynbuf_insert(&dyn, status, strlen(status));
    /* fill out bar with spaces */
    int i = 0;
    int space = state->tcols - strlen(status);
    while(i++ < space)
        dynbuf_insert(&dyn, " ", 1);
    dynbuf_insert(&dyn, "\x1b[27m\r\n", 7);
    write(STDOUT_FILENO, dyn.buf, dyn.ptr);
    dynbuf_free(&dyn);
}

/* draws the prompt below status bar */
static void prompt(fv_state *state)
{
    if (state->prompt_idx)
        write(STDOUT_FILENO, state->prompt, state->prompt_idx);
    else
        write(STDOUT_FILENO, "\x1b[K", 3);
    return ;
}

/* draw rows */
void draw_rows(fv_state *state)
{
    struct dynbuf dyn = DYNBUF_INIT;
    /* last 3 rows are for - padding, status bar, prompt */
    int rows = state->trows - 3;
    frow **contents = state->f.contents;
    int linenum_padding = state->f.line_count_digs;
    int line_count = state->f.line_count;
    int lines_drawn = 0;
    int i = state->voffset;
    while(lines_drawn < rows && i < line_count) {
        /* clear line */
        dynbuf_insert(&dyn, "\x1b[K", 3);
        /* draw line number */
        char num[linenum_padding + 3];
        sprintf(num, "%*d| ", linenum_padding, i + 1);
        int numlen = strlen(num);
        dynbuf_insert(&dyn, num, numlen);
        /* draw a line only if it should be visible */
        if (state->hoffset < contents[i]->len){
            /* draw line */
            int linelen = contents[i]->len - state->hoffset;
            if (linelen > state->tcols - numlen)
                linelen = state->tcols - numlen;
            dynbuf_insert(&dyn, contents[i]->line + state->hoffset, linelen);
        }
        /* draw newline and carriage return */
        dynbuf_insert(&dyn, "\r\n", 2);
        i++;
        lines_drawn++;
    }
    write(STDIN_FILENO, dyn.buf, dyn.ptr);
    dynbuf_free(&dyn);
}

/* refreshes terminal screen. This function is called on every valid input */
void refresh_screen(fv_state *state)
{
    /* hide cursor */
    write(STDOUT_FILENO, "\x1b[?25h", 6);
    /* move cursor to top left */
    move_cursor(0, 0);
    draw_rows(state);
    /* place cursor at the (bottom - 1) row of the screen */
    move_cursor(state->trows-1, 0);
    status_bar(state);
    prompt(state);
    /* move cursor to prompt */
    move_cursor(state->trows, state->prompt_idx+1);
    /* unhide cursor */
    write(STDOUT_FILENO, "\x1b[?25h", 6);
    return ;
}
