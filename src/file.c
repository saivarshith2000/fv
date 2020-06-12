#include <sys/stat.h>

#include "file.h"
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
static void insert_row(struct file *f, struct filerow *row)
{
    if (f->line_count == f->line_capacity) {
        f->contents = realloc(f->contents, sizeof(struct filerow*) * (f->line_capacity + ROW_STEP));
    }
    f->contents[f->line_count] = row;
    f->line_count++;
    return ;
}

/* Reads file and stores the lines read in the dynamic array 'contents' */
static int read_file(struct file *f)
{
    char *line = NULL;
    size_t linelen = 0;
    size_t linecap = 0;
    size_t line_count = 0;
    struct filerow *head = NULL;
    struct filerow *prev = NULL;
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
struct file* handle_file(char *filename)
{
    /* verfiy if file exists */
    verfiy_file(filename);
    /* try to open the file */
    FILE *fptr = fopen(filename, "r");
    if(fptr == NULL)
        DIE("fopen() failed");
    struct file *f = malloc(sizeof(struct file));
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
