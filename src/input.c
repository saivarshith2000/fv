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

/* macro to check if a char is numeric */
#define IS_NUM(ch) (ch >= '0' && ch <= '9')

/* scroll direction enum */
enum scroll_dir {
    SCR_LEFT,
    SCR_RIGHT,
    SCR_UP,
    SCR_DOWN
};

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

/* converts a string of digits to a number. If the string is invalid
 * ie., contains non numeric digits, -1 is returned. Otherwise, the
 * converted int is returned */
static int convert_to_num(char *str, int len)
{
    int ret = 0;
    int i = 0;
    while(i < len) {
        if (!IS_NUM(str[i]))
            return -1;
        ret = 10 * ret + str[i] - 48;
        i++;
    }
    return ret;
}

/* simple utility function to clear prompt */
static void clear_prompt(struct fv *cfg)
{
    memset(cfg->prompt, '\0', cfg->tcols);
    cfg->prompt_idx = 0;
}

/* adjusts voffset and hoffset to scroll file */
static void scroll(struct fv *cfg, int n, enum scroll_dir dir)
{
    int max_voffset;
    switch (dir) {
        case SCR_UP:
            if (cfg->f->line_count <= cfg->trows)
                return ;
            if (cfg->voffset < n)
                cfg->voffset = 0;
            else
                cfg->voffset -= n;
            return ;

        case SCR_DOWN:
            if (cfg->f->line_count <= cfg->trows)
                return ;
            max_voffset = cfg->f->line_count - cfg->trows + 3;
            if (cfg->voffset + n > max_voffset)
                cfg->voffset = max_voffset;
            else
                cfg->voffset += n;
            return ;

        case SCR_LEFT:
            if (cfg->f->max_linelen <= cfg->tcols - cfg->f->line_count_digs - 2)
                return ;
            if (cfg->hoffset < n)
                cfg->hoffset = 0;
            else
                cfg->hoffset -= n;
            return ;

        case SCR_RIGHT:
            if (cfg->f->max_linelen <= cfg->tcols - cfg->f->line_count_digs - 2)
                return  ;
            if (cfg->hoffset + n > cfg->f->max_linelen)
                cfg->hoffset = cfg->f->max_linelen;
            else
                cfg->hoffset += n;
            return ;
    }
}

/* handles basic input like movement keys (h, j, k, l, g, G) and quit */
static void handle_basic_input(int key, struct fv *cfg)
{
    switch(key) {
        case 'h':
            scroll(cfg, 1, SCR_LEFT);
            return ;

        case 'j':
            scroll(cfg, 1, SCR_DOWN);
            return ;

        case 'k':
            scroll(cfg, 1, SCR_UP);
            return ;

        case 'l':
            scroll(cfg, 1, SCR_RIGHT);
            return ;

        case 'g':
            /* goto to top */
            cfg->voffset = 0;
            return ;

        case 'G':
            /* goto to bottom */
            cfg->voffset = cfg->f->line_count - cfg->trows + 3;
            return ;

        case 'q':
            exit(EXIT_SUCCESS);
    }
}

/* handles user input accumilated in the prompt */
static void handle_search_input(int key, struct fv *cfg)
{
    if (key != '\r' && key != '\n') {
        cfg->prompt[cfg->prompt_idx++] = key;
    }
}

/* handles numeric input for movement */
static void handle_numeric_input(int key, struct fv *cfg)
{
    if (IS_NUM(key)) {
        cfg->prompt[cfg->prompt_idx++] = key;
        return ;
    }
    int n = convert_to_num(cfg->prompt, cfg->prompt_idx);
    if (n == -1) {
        /* invalid numeric input */
        clear_prompt(cfg);
        return ;
    }
    switch (key) {
        case '\r':
        case '\n':
            /* goto the nth line */
            cfg->voffset = 0;
            if (n != 0)
                scroll(cfg, n-1, SCR_DOWN);
            break;

        case 'h':
            scroll(cfg, n, SCR_LEFT);
            break;

        case 'j':
            scroll(cfg, n, SCR_DOWN);
            break;

        case 'k':
            scroll(cfg, n, SCR_UP);
            break;

        case 'l':
            scroll(cfg, n, SCR_RIGHT);
            break;
    }
    clear_prompt(cfg);
}

/* Obtains user input via read_key() and modifies the state of fv struct */
void process_input(struct fv *cfg)
{
    int key = read_key();

    /* if ESC is pressed, clear prompt */
    if (key == '\x1b') {
        clear_prompt(cfg);
        return ;
    }

    /* if prompt is empty */
    if (cfg->prompt_idx == 0) {
        if (IS_NUM(key)) {
            handle_numeric_input(key, cfg);
        } else if (key == '/') {
            handle_search_input(key, cfg);
        } else {
            handle_basic_input(key, cfg);
        }
        return ;
    }

    /* if prompt is not empty */
    if (cfg->prompt[0] == '/') {
        handle_search_input(key, cfg);
    } else if (IS_NUM(cfg->prompt[0])) {
        handle_numeric_input(key, cfg);
    } else {
        /* invalid input */
        clear_prompt(cfg);
    }
    return ;
}
