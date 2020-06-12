#include <string.h>
#include <stdlib.h>
#include "dynbuf.h"

#include <stdio.h>
#include <unistd.h>

/* inserts str into dynbuf. Returns 0 on success and -1 otherwise */
int dynbuf_insert(struct dynbuf *dyn, const char *str, int len)
{
    if (dyn == NULL || dyn->buf == NULL)
        return -1;
    if (dyn->size - dyn->ptr < len) {
        /* realloc() a new chunk if buffer runs out of memory */
        dyn->buf = realloc(dyn->buf, dyn->size + DYNBUF_CHUNK_SIZE);
        dyn->size += DYNBUF_CHUNK_SIZE;
    }
    memcpy(dyn->buf + dyn->ptr, str, len);
    dyn->ptr += len;
    return 0;
}

void dynbuf_free(struct dynbuf *dyn)
{
    free(dyn->buf);
}
