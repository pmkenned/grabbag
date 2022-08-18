#ifndef TIMER_H
#define TIMER_H

#if !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 199309L
#error "must define _POSIX_C_SOURCE >= 199309L"
#endif

/*
 * EXAMPLE:
 *
 *  DECLARE_TIMER(1);
 *
 *  START_TIMER(1);
 *
 *  my_func();
 *
 *  STOP_TIMER(1);
 *  printf("%ld\n", GET_ELAPSED_NS(1));
 *
 */

#include <time.h>

#define DECLARE_TIMER(X) \
    struct timespec start_time ## X, end_time ## X; \
    time_t dsec ## X; \
    long   dnsec ## X;

#define START_TIMER(X) \
    do {\
        clock_gettime(CLOCK_MONOTONIC, &start_time ## X);\
    } while(0)

#define STOP_TIMER(X) \
    do {\
        clock_gettime(CLOCK_MONOTONIC, &end_time ## X); \
        dsec ## X  = end_time ## X.tv_sec - start_time ## X.tv_sec; \
        dnsec ## X = end_time ## X.tv_nsec - start_time ## X.tv_nsec; \
    } while (0)

#define GET_ELAPSED_S(X)  (dsec ## X * 1000000000 + dnsec ## X)/1000000000
#define GET_ELAPSED_MS(X) (dsec ## X * 1000000000 + dnsec ## X)/1000000
#define GET_ELAPSED_US(X) (dsec ## X * 1000000000 + dnsec ## X)/1000
#define GET_ELAPSED_NS(X) (dsec ## X * 1000000000 + dnsec ## X)

#define PRINT_TIMER_S(X)  FPRINT_TIMER_S(stdout, X)
#define PRINT_TIMER_MS(X) FPRINT_TIMER_MS(stdout, X)
#define PRINT_TIMER_US(X) FPRINT_TIMER_US(stdout, X)
#define PRINT_TIMER_NS(X) FPRINT_TIMER_NS(stdout, X)

#define FPRINT_TIMER_S(STREAM, X)  fprintf(STREAM, "timer " #X ": %ld s\n",  GET_ELAPSED_S(X))
#define FPRINT_TIMER_MS(STREAM, X) fprintf(STREAM, "timer " #X ": %ld ms\n", GET_ELAPSED_MS(X))
#define FPRINT_TIMER_US(STREAM, X) fprintf(STREAM, "timer " #X ": %ld us\n", GET_ELAPSED_US(X))
#define FPRINT_TIMER_NS(STREAM, X) fprintf(STREAM, "timer " #X ": %ld ns\n", GET_ELAPSED_NS(X))

#endif /* TIMER_H */
