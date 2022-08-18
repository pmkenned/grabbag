#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#define NELEMS(X) (sizeof(X)/sizeof((X)[0]))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef struct {
    unsigned char * p;
    size_t n;
} buffer_t;

uint64_t pack_le(uint8_t * p, size_t n);
uint64_t pack_be(uint8_t * p, size_t n);
buffer_t read_file(const char * filename);
void hex_dump(buffer_t buf, FILE * fp);

#endif /* COMMON_H */
