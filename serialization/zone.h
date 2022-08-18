#ifndef ZONE_H
#define ZONE_H

#include "common.h"
#include <stdio.h>
#include <stddef.h>

typedef struct {
    int nalloc;
    size_t cap;
    size_t endptr;
    u8 buffer[1];
} zone_t;

size_t      to_align(size_t i, size_t align);

zone_t *    zone_create(size_t cap);
void        zone_destroy(zone_t * z);
void *      zone_alloc(zone_t * z, size_t n, size_t align);
void        zone_print(zone_t * z, FILE * fp);

#endif /* ZONE_H */
