#include <string.h>
#include <stdlib.h>
#include "dynbuf.h"

/* initialises the dynbuf. 0 is returned on success, -1 otherwise */
int dynbuf_init(struct dynbuf *dyn)
{
    if (dyn == NULL)
        return -1;
    dyn->buf = malloc(DYNBUF_CHUNK_SIZE);
    dyn->len = DYNBUF_CHUNK_SIZE;
    dyn->ptr = 0;
    return 0;
}

/* inserts str into dynbuf. Returns 0 on success and -1 otherwise */
int dynbuf_insert(struct dynbuf *dyn, const char *str, int len)
{
    if (dyn == NULL)
        return -1;
    if (dyn->len - dyn->ptr < len) {
        /* realloc() a new chunk if buffer runs out of memory */
        char *newbuf = realloc(dyn->buf, dyn->len + DYNBUF_CHUNK_SIZE);
        dyn->len += DYNBUF_CHUNK_SIZE;
        dyn->buf = newbuf;
    }
    memcpy(dyn->buf + dyn->ptr, str, len);
    dyn->ptr += len;
    return 0;
}

void dynbuf_free(struct dynbuf *dyn)
{
    free(dyn->buf);
    free(dyn);
}
