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

/* feature test macros for getline() */
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "fv_file.h"
#include "fv.h"

/* counts the number of digits in n */
static int count_digs(int n)
{
    int count = 0;
    while(n != 0) {
        count++;
        n /= 10;
    }
    return count;
}

/* checks if a REGULAR file 'filename' exists. Returns 0 on success
 * and -1 on return */
static int verfiy_file(char *filename)
{
    struct stat st;
    if (stat(filename, &st) != 0)
        return -1;
    if (S_ISREG(st.st_mode) == 0) {
        return -1;
    }
    return 0;
}

/* inserts a new row into the dynamic contents array */
static void insert_row(struct fv_file *f, frow *row)
{
    if (f->line_count == f->line_capacity) {
        struct frow **newmem = realloc(f->contents, sizeof(struct filerow*) * (f->line_capacity + ROW_STEP));
        if (newmem == NULL) {
            /* realloc failed */
            free(f->contents);
            free(f);
            exit(EXIT_FAILURE);
        }
        f->contents = newmem;
        f->line_capacity += ROW_STEP;
    }
    f->contents[f->line_count++] = row;
    return ;
}

/* Reads file and stores the lines read in the dynamic array 'contents' */
static int read_file(FILE *fptr, fv_file *f)
{
    char *line = NULL;
    size_t linelen = 0;
    size_t linecap = 0;
    size_t max_linelen = 0;
    while((linelen = getline(&line, &linecap, fptr)) != -1) {
        while(linelen > 0 && (line[linelen-1] == '\n' || line[linelen-1] == '\r'))
            linelen--;
        struct frow *row = malloc(sizeof(struct frow));
        row->line = malloc(linelen + 1);
        memcpy(row->line, line, linelen);
        row->line[linelen] = '\0';
        row->len = linelen + 1;
        insert_row(f, row);
        if (linelen > max_linelen)
            max_linelen = linelen;
    }
    f->max_linelen = max_linelen;
    f->line_count_digs = count_digs(f->line_count);
    free(line);
    return 0;
}

/* If filename exists, it attempts to read file and fill out the struct fv_file
 * Returns 0 on success and -1 on failure.
 * filename -> name of the file to read.
 * *f -> pointer to fv_file struct.
 */
int handle_file(char *filename, fv_file *f)
{
    if (f == NULL)
        return -1;

    /* verfiy if file exists */
    if (verfiy_file(filename) == -1) {
        fprintf(stderr, "File %s not found.\n", filename);
        return -1;
    }
    /* try to open the file */
    FILE *fptr = fopen(filename, "r");
    if(fptr == NULL) {
        fprintf(stderr, "Failed to open %s. fopen() failed.\n", filename);
        return -1;
    }
    f->filename_len = strlen(filename);
    f->contents = malloc(sizeof(struct filerow * ) * ROW_STEP);
    f->line_capacity = ROW_STEP;
    if(read_file(fptr, f) == -1) {
        fclose(fptr);
        fprintf(stderr, "Failed to read file %s\n", filename);
        return -1;
    }
    fclose(fptr);
    return 0;
}
