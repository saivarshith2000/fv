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

/* checks if a REGULAR file 'filename' exists */
static void verfiy_file(char *filename)
{
    struct stat st;
    if (stat(filename, &st) != 0)
        DIE("stat() failed");
    if (S_ISREG(st.st_mode) == 0) {
        printf("%s is not a regular file\n", filename);
        exit(EXIT_FAILURE);
    }
    return ;
}

/* inserts a new row into the dynamic contents array */
static void insert_row(struct fv_file *f, struct filerow *row)
{
    if (f->line_count == f->line_capacity) {
        struct filerow **newmem = realloc(f->contents, sizeof(struct filerow*) * (f->line_capacity + ROW_STEP));
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
static int read_file(struct fv_file *f)
{
    char *line = NULL;
    size_t linelen = 0;
    size_t linecap = 0;
    size_t max_linelen = 0;
    while((linelen = getline(&line, &linecap, f->fptr)) != -1) {
        while(linelen > 0 && (line[linelen-1] == '\n' || line[linelen-1] == '\r'))
            linelen--;
        struct filerow *row = malloc(sizeof(struct filerow));
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

/* reads the contents of file 'filename' and returns a struct file pointer */
struct fv_file* handle_file(char *filename)
{
    /* verfiy if file exists */
    verfiy_file(filename);
    /* try to open the file */
    FILE *fptr = fopen(filename, "r");
    if(fptr == NULL)
        DIE("fopen() failed");
    struct fv_file *f = malloc(sizeof(struct fv_file));
    f->filename = filename;
    f->filename_len = strlen(filename);
    f->fptr = fptr;
    f->contents = malloc(sizeof(struct filerow * ) * ROW_STEP);
    f->line_capacity = ROW_STEP;
    if(read_file(f) == -1) {
        free(f);
        printf("Failed to read file\n");
        exit(EXIT_FAILURE);
    }
    fclose(f->fptr);
    return f;
}
