// https://dirtyhandscoding.github.io/posts/performance-comparison-linear-search-vs-binary-search.html
// https://stackoverflow.com/questions/2741859/how-fast-can-you-make-linear-search
// https://schani.wordpress.com/2010/04/30/linear-vs-binary-search/
// https://github.com/stgatilov/linear-vs-binary-search

#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>

int linearX_search_sse (const int *arr, int n, int key) {
    __m128i *in_data = (__m128i*)arr;
    __m128i key4 = _mm_set1_epi32(key);
    intptr_t i = 0;
    int res;
    for (;;) {
        __m128i cmp0 = _mm_cmpgt_epi32(key4, in_data [i + 0]);
        __m128i cmp1 = _mm_cmpgt_epi32(key4, in_data [i + 1]);
        __m128i cmp2 = _mm_cmpgt_epi32(key4, in_data [i + 2]);
        __m128i cmp3 = _mm_cmpgt_epi32(key4, in_data [i + 3]);
        __m128i pack01 = _mm_packs_epi32(cmp0, cmp1);
        __m128i pack23 = _mm_packs_epi32(cmp2, cmp3);
        __m128i pack0123 = _mm_packs_epi16 (pack01, pack23);
        res = _mm_movemask_epi8 (pack0123);
        if (res != 0xFFFF)
            break;
        i += 4;
    }

    // bsf: Bit Scan Forward
    //return i * 4 + bsf(~res);

    // ctz: count trialing zeros
    return i * 4 + __builtin_ctz(~res);
}

int main()
{
    int x[] = {7, 2, 5, 1, 9, 4, 2, 6};
    int index = linearX_search_sse(x, 8, 9);
    printf("%d\n", index);
    return 0;
}
