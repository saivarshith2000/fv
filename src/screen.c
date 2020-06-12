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

#include "screen.h"
#include "dynbuf.h"

/* clear screen */
void clear_screen()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
}

/* refreshes terminal screen. This function is called on every valid input */
void refresh_screen(int trows, int tcols, int line_count, int voffset, struct filerow **contents)
{
    struct dynbuf dyn = DYNBUF_INIT;
    dynbuf_insert(&dyn, "\x1b[?25l", 6);
    dynbuf_insert(&dyn, "\x1b[H", 3);
    int i = 0;
    int min = line_count - voffset < trows ? line_count - voffset : trows;
    while(i < min && (i + voffset) < line_count) {
        struct filerow *row = contents[i + voffset];
        dynbuf_insert(&dyn, row->line, (row->len > tcols ? tcols : row->len));
        if (i == min - 1){
            dynbuf_insert(&dyn, "\x1b[K", 3);
        } else {
            dynbuf_insert(&dyn, "\x1b[K\r\n", 5);
        }
        i++;
    }
    dynbuf_insert(&dyn, "\x1b[H", 3);
    dynbuf_insert(&dyn, "\x1b[?25h", 6);
    write(STDOUT_FILENO, dyn.buf, dyn.ptr);
    return ;
}
