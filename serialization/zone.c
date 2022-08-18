#include "zone.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

zone_t * 
zone_create(size_t cap)
{
    zone_t * z = calloc(sizeof(*z)-1+cap, 1);
    z->cap = cap;
    return z;
}

void
zone_destroy(zone_t * z)
{
    free(z);
}

size_t
to_align(size_t i, size_t align)
{
    size_t bump = 0;
    if (i % align != 0)
        bump = align - (i % align);
    return bump;
}

void *
zone_alloc(zone_t * z, size_t n, size_t align)
{
    z->endptr += to_align(z->endptr, align);
    u8 * rv = z->buffer + z->endptr;
    if (rv >= z->buffer + z->cap) {
        assert(0);
        abort();
        return NULL;
    }
    z->nalloc++;
    z->endptr += n;
    return rv;
}

void
zone_print(zone_t * z, FILE * fp)
{
    for (size_t i = 0; i < (z->endptr+16)/16; i++) {
        fprintf(fp, "%4lx: ", i*16);
        for (size_t j = 0; j < 16; j++) {
            size_t idx = i*16+j;
            if (idx > z->endptr)
                break;
            fprintf(fp, "%02x ", z->buffer[idx]);
        }
        fprintf(fp, "\n");
    }
}
