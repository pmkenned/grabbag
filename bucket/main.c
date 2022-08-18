// https://hillelwayne.com/post/cleverness/

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "timer.h"

#define NELEMS(X) sizeof(X)/sizeof(X[0])
#define UNUSED(X) ((void)(X))
#define THOUSAND(X) X ## 000
#define MILLION(X) X ## 000000

DECLARE_TIMER(1);

static size_t num_bytes_allocated;
static size_t num_reallocs;
static void *
_realloc(void *ptr, size_t size)
{
    num_reallocs++;
    num_bytes_allocated += size;
    return realloc(ptr, size);
}

static void *
_malloc(size_t size)
{
    num_bytes_allocated += size;
    return malloc(size);
}

enum { INIT_BUCKET_CAP = 2200 };

typedef struct {
    size_t n;
    size_t cap;
    int _xs[INIT_BUCKET_CAP];
    int * xs;
} Bucket;

static void
bucket_add(Bucket * bp, int x)
{
    if (bp->n == bp->cap) {
        if (bp->cap == INIT_BUCKET_CAP) {
            bp->xs = NULL;
        }
        bp->cap *= 2;
        bp->xs = _realloc(bp->xs, sizeof(*bp->xs)*bp->cap);
        if (bp->cap == INIT_BUCKET_CAP*2) {
            memcpy(bp->xs, bp->_xs, sizeof(bp->_xs));
        }
    }
    bp->xs[bp->n++] = x;
}

static void
bucket_destroy(Bucket b)
{
    if (b.cap > INIT_BUCKET_CAP) {
        free(b.xs);
    }
}

enum { N_STATIC_BUCKETS = 10 };
static Bucket static_buckets[N_STATIC_BUCKETS];

static void
insertion_sort(int * xs, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        const int t = xs[i];
        size_t j;
        for (j = i; j != 0; j--) {
            if (t >= xs[j-1])
                break;
            xs[j] = xs[j-1];
        }
        xs[j] = t;
    }
}

static void
bucket_sort(int * x, size_t n, size_t k)
{
    assert(k);
    if (n == 0)
        return;
    assert(x);

    // TODO: decide on k based on n and range

    Bucket * buckets;

    // allocate buckets
    if (k <= N_STATIC_BUCKETS) {
        buckets = static_buckets;
    } else {
        buckets = _malloc(sizeof(*buckets)*k);
    }

    // init buckets
    for (size_t i = 0; i < k; i++) {
        buckets[i] = (Bucket) {.cap = INIT_BUCKET_CAP, .xs = buckets[i]._xs};
    }
    STOP_TIMER(1);
    fprintf(stdout, "init buckets: %ldms\n", GET_ELAPSED_MS(1));

    int min_x = x[0];
    int max_x = x[0];
    for (size_t i = 1; i < n; i++) {
        if (x[i] > max_x)
            max_x = x[i];
        if (x[i] < min_x)
            min_x = x[i];
    }
    if (max_x == min_x)
        return;

    const size_t bucket_size = (max_x-min_x)/k + 1;

    // add elements to buckets
    for (size_t i = 0; i < n; i++) {
        size_t bi = (x[i]-min_x) / bucket_size;
        bucket_add(buckets + bi, x[i]);
    }
    STOP_TIMER(1);
    fprintf(stdout, "fill buckets: %ldms\n", GET_ELAPSED_MS(1));

    // sort buckets
    for (size_t bi = 0; bi < k; bi++) {
        insertion_sort(buckets[bi].xs, buckets[bi].n);
    }
    STOP_TIMER(1);
    fprintf(stdout, "sort buckets: %ldms\n", GET_ELAPSED_MS(1));

    for (size_t bi = 0, i = 0; bi < k; bi++) {
#if 0
        fprintf(stderr, "bucket %zu size: %zu\n", bi, buckets[bi].n);
#endif
        for (size_t j = 0; j < buckets[bi].n; j++) {
            x[i++] = buckets[bi].xs[j];
        }
    }
    STOP_TIMER(1);
    fprintf(stdout, "copy buckets: %ldms\n", GET_ELAPSED_MS(1));

    for (size_t i = 0; i < k; i++) {
        bucket_destroy(buckets[i]);
    }

    if (k > N_STATIC_BUCKETS) {
        free(buckets);
    }
}

static void
print_array(int * xs, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        fprintf(stdout, "%d ", xs[i]);
    }
    fprintf(stdout, "\n");
}

typedef int (*cmpfn)(const void *, const void *);

static int
cmpint(const void * a, const void * b)
{
    int * aip = (int *) a;
    int * bip = (int *) b;
    return *aip - *bip;
}

static void
swap(void * a, void * b, size_t size)
{
    static char t[128];
    assert(size < sizeof(t));
    memcpy(t, a, size);
    memcpy(a, b, size);
    memcpy(b, t, size);
}

static size_t
partition(void * xs, size_t nmemb, size_t size, cmpfn cmp)
{
    void * pivot = (char *)xs + size*(nmemb-1);
    size_t i = 0;
    for (size_t j = 0; j < nmemb-1; j++) {
        if (cmp((char *)xs + j*size, pivot) <= 0) {
            swap((char *)xs + size*i, (char *)xs + size*j, size);
            i++;
        }
    }
    swap((char *)xs + size*i, (char *)xs + size*(nmemb-1), sizeof(int));
    return i;
}

static void
quick_sort(void * base, size_t nmemb, size_t size, cmpfn cmp)
{
    if (nmemb <= 1)
        return;
    size_t pi = partition(base, nmemb, size, cmp);
    if (pi > 0)
        quick_sort(base, pi - 1, size, cmp);
    quick_sort((char *)base + pi*size, nmemb - pi, size, cmp);
}

int main(int argc, char * argv[])
{
    // TODO: command-line arguments for array dimension, range, number of buckets, seed, number of iterations
    int niter = 1;
    int use_bucket_sort = 1,
        use_quick_sort = 0;
    if (argc > 1) {
        if (strcmp(argv[1], "bucket") == 0) {
            use_bucket_sort = 1;
        } else if (strcmp(argv[1], "quick") == 0) {
            use_bucket_sort = 0;
            use_quick_sort = 1;
        } else if (strcmp(argv[1], "qsort") == 0) {
            use_bucket_sort = 0;
            use_quick_sort = 0;
        } else {
            fprintf(stderr, "usage: %s [FUNC]\n"
                            "where FUNC is one of:\n"
                            "  bucket (default)\n"
                            "  quick\n"
                            "  qsort\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    enum {
        US_POP = MILLION(3),
        RANGE = 365*120,
    };
    srand(time(NULL));

    size_t n = US_POP;
    size_t k = 45000;
    int * xs = malloc(sizeof(*xs)*n);
    for (int j = 0; j < niter; j++) {

        // create random array
        fprintf(stdout, "creating random array...\n");
        for (size_t i = 0; i < n; i++) {
            xs[i] = rand() % RANGE;
        }
        fprintf(stdout, "n: %zu\nk: %zu\n", n, k);

        // sort using chosen algorithm
        START_TIMER(1);
        if (use_bucket_sort) {
            fprintf(stdout, "sorting using bucket sort...\n");
            bucket_sort(xs, n, k);
        } else if (use_quick_sort) {
            fprintf(stdout, "sorting using quick sort...\n");
            //print_array(xs, n);
            quick_sort(xs, n, sizeof(*xs), cmpint);
            print_array(xs, 1000);
        } else {
            fprintf(stdout, "sorting using qsort...\n");
            qsort(xs, n, sizeof(*xs), cmpint);
        }
        STOP_TIMER(1);
        fprintf(stdout, "sorting time: %ldms\n", GET_ELAPSED_MS(1));

        free(xs);
    }

    if (use_bucket_sort) {
        fprintf(stdout, "number of realloc calls: %zu\n", num_reallocs);
        fprintf(stdout, "number of bytes allocated: %zu\n", num_bytes_allocated);
    }

    return 0;
}
