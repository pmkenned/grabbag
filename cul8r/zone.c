#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#define NELEMS(X) sizeof(X)/sizeof(X[1])
#define RAND_ITEM(X) X[rand() % NELEMS(X)]

#define N 4000000

typedef struct zone_t zone_t;

struct zone_t {
    void * base;
    size_t cap;
    uintptr_t p;
    zone_t * next;
};

static int num_zones = 0;

static zone_t
zone_init(size_t cap)
{
    num_zones++;
    zone_t z;
    z.cap = cap;
    z.base = malloc(cap);
    z.p = (uintptr_t) z.base;
    z.next = NULL;
    //fprintf(stderr, "allocated new zone at %p\n", z.base);
    return z;
}

static void
zone_deinit(zone_t * zp)
{
    //fprintf(stderr, "destroying zone... %p\n", zp->base);
    if (zp->next) {
        zone_deinit(zp->next);
    }
    free(zp->base);
}

void
zone_dump(FILE * fp, zone_t * zp)
{
    fprintf(fp, "dump of zone at %p:\n", zp->base);
    uint8_t * p = zp->base;
    for (size_t i = 0; i < zp->cap; i += 16) {
        fprintf(fp, "%04lx: ", i);
        for (int j = 0; j < 16; j++) {
            fprintf(fp, "%02x ", *p++);
        }
        fprintf(fp, "\n");
    }
    if (zp->next) {
        zone_dump(fp, zp->next);
    }
}

#if 0
static int
zone_sum(zone_t * zp)
{
    int sum = 0;
    uint8_t * p = zp->base;
    for (size_t i = 0; i < zp->cap; i += 4) {
        p += 4;
        sum += *(int*)p;
    }
    if (zp->next) {
        sum += zone_sum(zp->next);
    }
    return sum;
}
#endif

static void *
zone_alloc_align(zone_t * zp, size_t sz, size_t align)
{
    zp->p += (zp->p % align == 0) ? 0 : align - (zp->p % align);
    void * old_p = (void *) zp->p;
    if (zp->p + sz < (uintptr_t) zp->base + zp->cap) {
        zp->p += sz;
        return old_p;
    }
    // this zone is full
#if 1
    zone_t * new_zone = malloc(sizeof(*new_zone));
    *new_zone = *zp;
    *zp = zone_init(zp->cap);
    zp->next = new_zone;
    return zone_alloc_align(zp, sz, align);
#else
    if (zp->next == NULL) {
        zp->next = malloc(sizeof(*zp->next));
        *zp->next = zone_init(zp->cap);
    }
    return zone_alloc_align(zp->next, sz, align);
#endif
}

static void *
zone_alloc(zone_t * zp, size_t sz)
{
    size_t align = sz > 16 ? 16 : sz;
    return zone_alloc_align(zp, sz, align);
}

void * ptrs[N];

int main()
{
    size_t sizes[] = {4, 16, 32, 128};
    zone_t z = zone_init(1<<22);
    size_t total = 0;
    int pi = 0;
    for (int i = 0; i < N; i++) {
        size_t sz = RAND_ITEM(sizes);
        //printf("%zu\n", sz);
        uint8_t * xp = zone_alloc(&z, sz);
        *xp = sz;
        ptrs[pi++] = xp;
        total += sz;
        if ((i+1) % 10 == 0) {
            for (int j = 0; j < 5; j++) {
                size_t r = rand() % pi;
                ptrs[r] = ptrs[--pi];
            }
        }
    }
    printf("total allocated: %zu\n", total);
    printf("num zones: %d\n", num_zones);
    //zone_dump(stdout, &z);
    zone_deinit(&z);
    return 0;
}
