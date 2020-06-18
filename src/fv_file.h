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

#ifndef _FILE_H_
#define _FILE_H_

#define _GNU_SOURCE
#include <stdio.h>

#define LINENUM_PAD_CHARS 4     /* padding characters around line number */

struct frow {
    char *line;
    size_t len;
};
typedef struct frow frow;

struct fv_file {
    unsigned int filename_len;           /* strlen() of the filename */
    unsigned int line_count;             /* total number of lines in the file */
    unsigned int linenum_digs;           /* number of digits in line count */
    unsigned int max_linelen;            /* maximum width of all the lines in the file */
    frow **contents;                     /* dynamic array of filerows */
    unsigned int contents_cap;           /* current capacity of the contents array */
};
typedef struct fv_file fv_file;

int handle_file(char *filename, fv_file *f);

#endif /* _FV_H_ */
