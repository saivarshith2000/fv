#ifndef _DYNBUF_H_
#define _DYNBUF_H_

#define DYNBUF_CHUNK_SIZE 64

/* Dynamic character buffer data structure */
struct dynbuf {
    char *buf;   /* pointer to buffer */
    int ptr;     /* pointer to the first empty space in buffer */
    int len;     /* length of buffer */
};

/* prototypes */
int dynbuf_init(struct dynbuf *dyn);
int dynbuf_insert(struct dynbuf *dynbuf, const char *str, int len);
void dynbuf_free(struct dynbuf *dyn);
#endif
