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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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
 * and errors. Returns the key pressed on success and -1 on failure.
 */
static int read_key()
{
    char c;
    int br;
    while((br = read(STDIN_FILENO, &c, 1)) != 1) {
        if (br == -1 && errno != EAGAIN) {
            /* If any error other than timeout occurs, return -1 */
            return -1;
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
static void clear_prompt(fv_state *state)
{
    memset(state->prompt, '\0', state->tcols);
    state->prompt_idx = 0;
}

/* adjusts voffset and hoffset to scroll file */
static void scroll(fv_state *state, int n, enum scroll_dir dir)
{
    int max_voffset;
    switch (dir) {
        case SCR_UP:
            if (state->f.line_count <= state->trows)
                return ;
            if (state->voffset < n)
                state->voffset = 0;
            else
                state->voffset -= n;
            return ;

        case SCR_DOWN:
            if (state->f.line_count <= state->trows)
                return ;
            max_voffset = state->f.line_count - state->trows + 3;
            if (state->voffset + n > max_voffset)
                state->voffset = max_voffset;
            else
                state->voffset += n;
            return ;

        case SCR_LEFT:
            if (state->f.max_linelen <= state->tcols - state->f.line_count_digs - 2)
                return ;
            if (state->hoffset < n)
                state->hoffset = 0;
            else
                state->hoffset -= n;
            return ;

        case SCR_RIGHT:
            if (state->f.max_linelen <= state->tcols - state->f.line_count_digs - 2)
                return  ;
            if (state->hoffset + n > state->f.max_linelen)
                state->hoffset = state->f.max_linelen;
            else
                state->hoffset += n;
            return ;
    }
}

/* handles basic input like movement keys (h, j, k, l, g, G) and quit */
static void handle_basic_input(int key, fv_state *state)
{
    switch(key) {
        case 'h':
            scroll(state, 1, SCR_LEFT);
            return ;

        case 'j':
            scroll(state, 1, SCR_DOWN);
            return ;

        case 'k':
            scroll(state, 1, SCR_UP);
            return ;

        case 'l':
            scroll(state, 1, SCR_RIGHT);
            return ;

        case 'g':
            /* goto to top */
            state->voffset = 0;
            return ;

        case 'G':
            /* goto to bottom */
            state->voffset = state->f.line_count - state->trows + 3;
            return ;

        case 'q':
            quit(state, NULL, EXIT_SUCCESS, 1);
    }
}

/* handles user input accumilated in the prompt */
static void handle_search_input(int key, fv_state *state)
{
    if (key != '\r' && key != '\n') {
        state->prompt[state->prompt_idx++] = key;
    }
}

/* handles numeric input for movement */
static void handle_numeric_input(int key, fv_state *state)
{
    if (IS_NUM(key)) {
        state->prompt[state->prompt_idx++] = key;
        return ;
    }
    int n = convert_to_num(state->prompt, state->prompt_idx);
    if (n == -1) {
        /* invalid numeric input */
        clear_prompt(state);
        return ;
    }
    switch (key) {
        case '\r':
        case '\n':
            /* goto the nth line */
            state->voffset = 0;
            if (n != 0)
                scroll(state, n-1, SCR_DOWN);
            break;

        case 'h':
            scroll(state, n, SCR_LEFT);
            break;

        case 'j':
            scroll(state, n, SCR_DOWN);
            break;

        case 'k':
            scroll(state, n, SCR_UP);
            break;

        case 'l':
            scroll(state, n, SCR_RIGHT);
            break;
    }
    clear_prompt(state);
}

/* Obtains user input via read_key() and modifies the state of fv struct */
void process_input(fv_state *state)
{
    int key = read_key();

    /* if ESC is pressed, clear prompt */
    if (key == '\x1b') {
        clear_prompt(state);
        return ;
    }

    /* if prompt is empty */
    if (state->prompt_idx == 0) {
        if (IS_NUM(key)) {
            handle_numeric_input(key, state);
        } else if (key == '/') {
            handle_search_input(key, state);
        } else {
            handle_basic_input(key, state);
        }
        return ;
    }

    /* if prompt is not empty */
    if (state->prompt[0] == '/') {
        handle_search_input(key, state);
    } else if (IS_NUM(state->prompt[0])) {
        handle_numeric_input(key, state);
    } else {
        /* invalid input */
        clear_prompt(state);
    }
    return ;
}
