#ifndef _DYNBUF_H_
#define _DYNBUF_H_

#include <stdlib.h>

#define DYNBUF_CHUNK_SIZE 1024

/* Dynamic character buffer data structure */
struct dynbuf {
    char *buf;   /* pointer to buffer */
    int ptr;     /* pointer to the first empty space in buffer */
    int size;     /* length of buffer */
};

/* simple macro to init a dynbuf */
#define DYNBUF_INIT {malloc(DYNBUF_CHUNK_SIZE), 0, DYNBUF_CHUNK_SIZE}

/* prototypes */
int dynbuf_insert(struct dynbuf *dynbuf, const char *str, int len);
void dynbuf_free(struct dynbuf *dyn);

#endif /* _DYNBUF_H_ */
