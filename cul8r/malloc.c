#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#define NELEMS(X) sizeof(X)/sizeof(X[1])
#define RAND_ITEM(X) X[rand() % NELEMS(X)]

#define N 4000000

void * ptrs[N];

int main()
{
    size_t sizes[] = {4, 16, 32, 128};
    size_t total = 0;
    int pi = 0;
    for (int i = 0; i < N; i++) {
        size_t sz = RAND_ITEM(sizes);
        //printf("%zu\n", sz);
        uint8_t * xp = malloc(sz);
        *xp = i;
        ptrs[pi++] = xp;
        total += sz;
#if 1
        if ((i+1) % 10 == 0) {
            for (int j = 0; j < 5; j++) {
                size_t r = rand() % pi;
                free(ptrs[r]);
                ptrs[r] = ptrs[--pi];
            }
        }
#endif
    }

    printf("total allocated: %zu\n", total);

    for (int i = 0; i < pi; i++) {
        free(ptrs[i]);
    }
    return 0;
}
