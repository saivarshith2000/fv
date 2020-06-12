#ifndef _FILE_H_
#define _FILE_H_

#define _GNU_SOURCE
#include <stdio.h>

#define ROW_STEP 64

struct filerow {
    char *line;
    size_t len;
};

struct file {
    FILE *fptr;
    int line_count;
    int line_capacity;
    struct filerow **contents;
};

struct file *handle_file(char *filename);

#endif
