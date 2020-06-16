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

#ifndef _FV_H_
#define _FV_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "fv_file.h"

/* A simple exit macro which displays an error message and exits */
#define DIE(msg) {\
    fprintf(stderr, "ERROR: %s\n", msg);\
    fprintf(stderr, "AT: %d in %s\n", __LINE__, __FILE__);\
    fprintf(stderr, "ERRNO: %d (%s)", errno, strerror(errno)); \
    exit(EXIT_FAILURE);}

/* the fv struct contains all the terminal and file variables in one place */
struct fv {
    struct termios orig;     /* termios struct before going into raw mode */
    int trows, tcols;        /* rows and columns of the terminal screen */
    char *filename;          /* name of the file open with fv */
    struct fv_file *f;       /* pointer to the file struct. see src/file.h */
    int voffset;             /* vertical offset. Used in vertical scrolling */
    int hoffset;             /* horizontal offset. Used in horizontal scrolling */
    char *prompt;            /* prompt below status bar */
    int prompt_idx;          /* index of the next character in prompt */
};

#endif /* _FV_H_ */
