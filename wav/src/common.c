#define _XOPEN_SOURCE 500
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>

uint64_t
pack_le(uint8_t * p, size_t n) {
    assert(n <= 8);
    size_t i = 0;
    uint64_t rv = 0;
    while (n-- > 0)
        rv += *p++ << (i++*8);
    return rv;
}

uint64_t
pack_be(uint8_t * p, size_t n) {
    assert(n <= 8);
    uint64_t rv = 0;
    while (n-- > 0) {
        rv <<= 8;
        rv += (*p++);
    }
    return rv;
}

buffer_t
read_file(const char * filename)
{
    buffer_t buffer;
    struct stat statbuf;

    if (lstat(filename, &statbuf) != 0) {
        perror("fstat");
        exit(1);
    }

    off_t file_size = statbuf.st_size;

    FILE * fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror(filename);
        exit(1);
    }

    buffer.p = malloc(file_size);
    buffer.n = file_size;

    fread(buffer.p, 1, file_size, fp);

    if (ferror(fp)) {
        perror("fread");
        exit(1);
    }

    fclose(fp);

    return buffer;
}

void
hex_dump(buffer_t buf, FILE * fp)
{
    int i, j;
    size_t addr = 0;
    for (i = 0; i < 50; i++) {
        fprintf(fp, "%08lx: ", addr);
        size_t prev_addr = addr;
        for (j = 0; j < 16; j++, addr++) {
            if (addr >= buf.n)
                break;
            fprintf(fp, "%02x ", buf.p[addr]);
        }
        addr = prev_addr;
        for (j = 0; j < 16; j++, addr++) {
            if (addr >= buf.n)
                break;
            unsigned char c = buf.p[addr];
            if (isprint(c))
                fprintf(fp, "%c", c);
            else
                fprintf(fp, ".");
        }
        fprintf(fp, "\n");
        if (addr >= buf.n)
            break;
    }
}
