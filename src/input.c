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
void process_input(struct fv *config)
{
    int key = read_key();
    switch(key) {
        case 'j':
            {
                /* scroll down 1 line */
                if (config->voffset < config->f->line_count)
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
        case 'q':
            {
                /* quit */
                exit(0);
            }
    }
}
