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
void refresh_screen(int trows, int tcols, int line_count, struct filerow *contents)
{
    struct dynbuf dyn = DYNBUF_INIT;
    dynbuf_insert(&dyn, "\x1b[?25l", 6);
    dynbuf_insert(&dyn, "\x1b[H", 3);
    struct filerow *row = contents;
    int i = 0;
    while(i < trows-1 && row != NULL) {
        dynbuf_insert(&dyn, row->line, (row->len > tcols ? tcols : row->len));
        dynbuf_insert(&dyn, "\r\n\x1b[K", 5);
        row = row->next;
        i++;
    }
    dynbuf_insert(&dyn, "\x1b[H", 3);
    dynbuf_insert(&dyn, "\x1b[?25h", 6);
    write(STDOUT_FILENO, dyn.buf, dyn.ptr);
    return ;
}
