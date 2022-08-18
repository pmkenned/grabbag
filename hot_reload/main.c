#define _POSIX_C_SOURCE 200809L
#include "state.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <ncurses.h>

#define UNUSED(x) (void)(x)

#define MS(x) x ## 000000L

#define DECLARE_TIMER(X) \
    struct timespec start_time ## X, end_time ## X; \
    time_t dsec ## X; \
    long   dnsec ## X;

#define UNUSED_TIMER(X) \
    UNUSED(start_time ## X); \
    UNUSED(end_time ## X); \
    UNUSED(dsec ## X); \
    UNUSED(dnsec ## X);

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

#define PRINT_TIMER(X) FPRINT_TIMER(stdout, X)

#define FPRINT_TIMER(STREAM, X) \
        fprintf(STREAM, "timer " #X ": %ld us\n", (dsec ## X * 1000000000 + dnsec ## X)/1000);

state_t mem;

void (*update)(state_t * mem);

const char * libname = "libshared.so";

void * lib_handle = NULL;

static void
load_lib()
{
    DECLARE_TIMER(0);
    UNUSED_TIMER(0);
    char * error;
    if (lib_handle != NULL)
        dlclose(lib_handle);

    //fprintf(stderr, "loading...\n");
    START_TIMER(0);
    lib_handle = dlopen(libname, RTLD_LAZY);
    if (!lib_handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }
    dlerror(); /* Clear any existing error */
    update = dlsym(lib_handle, "update");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }

    STOP_TIMER(0);
    //fprintf(stderr, "done.\n");
    //FPRINT_TIMER(stderr, 0);
}


static int
lib_changed()
{
    int changed = 0;
    static int prev_time_valid = 0;
    static struct timespec prev_mtim;
    struct stat statbuf;
    if (lstat("sentinel", &statbuf) != 0) {
        perror("");
        exit(1);
    }
    if (prev_time_valid && (prev_mtim.tv_nsec != statbuf.st_mtim.tv_nsec)) {
        //printf("changed\n");
        changed = 1;
    }
    prev_mtim = statbuf.st_mtim;
    prev_time_valid = 1;
    return changed;
}


int main()
{
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = MS(100);

    load_lib();

    initscr();
    mem.dir = DIR_RIGHT;
    nodelay(stdscr, true);
    while (1) {

        if (getch() == 'q')
            break;
        (*update)(&mem);
        refresh();

        nanosleep(&ts, NULL);
        if (lib_changed())
            load_lib();
    }
    endwin();

    dlclose(lib_handle);
    return 0;
}
