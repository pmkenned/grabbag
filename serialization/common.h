#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdlib.h>

#define NELEMS(X) sizeof(X)/sizeof((X)[0])

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t    s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

#define STR(X) #X
#define XSTR(X) STR(X)

#define ERROR(S)            fprintf(stderr, "ERROR: %s", S)
#define ERRORF(fmt, ...)    fprintf(stderr, "ERROR:" fmt, __VA_ARGS__)
#define FL_ERROR(S)         fprintf(stderr, "ERROR: " __FILE__ ":" XSTR(__LINE__) ":%s", S)
#define FL_ERRORF(fmt, ...) fprintf(stderr, "ERROR: " __FILE__ ":" XSTR(__LINE__) ":" fmt, __VA_ARGS__)

#define FAIL(S)             do { ERROR(S);                      exit(EXIT_FAILURE); } while (0)
#define FAILF(fmt, ...)     do { ERRORF(fmt, __VA_ARGS__);      exit(EXIT_FAILURE); } while (0)
#define FL_FAIL(S)          do { FL_ERROR(S);                   exit(EXIT_FAILURE); } while (0)
#define FL_FAILF(fmt, ...)  do { FL_ERRORF(fmt, __VA_ARGS__);   exit(EXIT_FAILURE); } while (0)

#define PFAIL(S)            do {                                                    perror(S);  exit(EXIT_FAILURE); } while (0)
#define PFAILF(fmt, ...)    do { ERRORF(fmt, __VA_ARGS__);                          perror(""); exit(EXIT_FAILURE); } while (0)
#define FL_PFAIL(S)         do { fputs(__FILE__ ":" XSTR(__LINE__) ":", stderr);    perror(S);  exit(EXIT_FAILURE); } while (0)
#define FL_PFAILF(fmt, ...) do { FL_ERRORF(fmt, __VA_ARGS__);                       perror(""); exit(EXIT_FAILURE); } while (0)


#endif /* COMMON_H */
