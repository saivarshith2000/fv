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
        char *newmem = realloc(dyn->buf, dyn->size + DYNBUF_CHUNK_SIZE);
        if (newmem == NULL) {
            /* realloc failed */
            free(dyn->buf);
            free(dyn);
            exit(EXIT_FAILURE);
        }
        dyn->buf = newmem;
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
