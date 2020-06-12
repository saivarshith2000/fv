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

/* reads the contents of the file and builds a linked list of struct filerow.
 * The head of linked list is stored in struct file's contents field. For now
 * getline() is used to read lines. If any portability issues come up, it will
 * be changed to a custom implementation of getline() later. Returns 0 on
 * success and -1 on other cases. Error messages are printed on errors.
 */
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
        row->next = NULL;
        if (head == NULL) {
            head = row;
            prev = head;
        } else {
            prev->next = row;
            prev = row;
        }
        line_count++;
    }
    free(line);
    f->contents = head;
    f->line_count = line_count;
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
    if(read_file(f) == -1) {
        free(f);
        printf("Failed to read file\n");
        exit(EXIT_FAILURE);
    }
    fclose(f->fptr);
    return f;
}
