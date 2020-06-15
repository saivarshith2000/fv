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

/* handles basic input like movement keys (j, k, g, G) and quit */
static void handle_basic_input(int key, struct fv *cfg)
{
    switch(key) {
        case 'j':
            scroll_down(cfg, 1);
            break;

        case 'k':
            scroll_up(cfg, 1);
            break;

        case 'g':
            /* goto to top */
            cfg->voffset = 0;
            break;

        case 'G':
            /* goto to bottom */
            cfg->voffset = cfg->f->line_count - cfg->trows + 3;
            break;

        case 'q':
            exit(EXIT_SUCCESS);
            break;
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
            if (n == 0)
                break;
            scroll_down(cfg, n-1);
            break;

        case 'j':
            /* scroll down n lines */
            scroll_down(cfg, n);
            break;

        case 'k':
            /* scroll up n lines */
            scroll_up(cfg, n);
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
