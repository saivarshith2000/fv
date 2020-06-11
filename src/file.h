#ifndef _FILE_H_
#define _FILE_H_

#define _GNU_SOURCE
#include <stdio.h>

struct filerow {
    char *line;
    size_t len;
    struct filerow *next;
};

struct file {
    FILE *fptr;
    int line_count;
    struct filerow *contents;
};

struct file *handle_file(char *filename);

#endif
