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

#include <sys/stat.h>

#include "fv_file.h"
#include "fv.h"

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
        f->contents = realloc(f->contents, sizeof(struct filerow*) * (f->line_capacity + ROW_STEP));
    }
    f->contents[f->line_count] = row;
    f->line_count++;
    return ;
}

/* Reads file and stores the lines read in the dynamic array 'contents' */
static int read_file(struct fv_file *f)
{
    char *line = NULL;
    size_t linelen = 0;
    size_t linecap = 0;
    while((linelen = getline(&line, &linecap, f->fptr)) != -1) {
        while(linelen > 0 && (line[linelen-1] == '\n' || line[linelen-1] == '\r'))
            linelen--;
        struct filerow *row = malloc(sizeof(struct filerow));
        row->line = malloc(linelen + 1);
        memcpy(row->line, line, linelen);
        row->line[linelen] = '\0';
        row->len = linelen + 1;
        insert_row(f, row);
    }
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
