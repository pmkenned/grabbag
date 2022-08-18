// NOTES:
//  struct initialization
//  static_assert
//  _Generic
//  defer
//  pass structs by value
//  anonymous struct members
//  valid member (or error code) on struct for monadic interfaces
//  request allocators or buffers
//  don't call OS
//  differentiate between temporary and long-lived allocations
//  logger API
//  hmm

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>

#define XSTR(X) STR(X)
#define STR(X) #X

#define CONCAT_(prefix, suffix) prefix##suffix
    /// Concatenate `prefix, suffix` into `prefixsuffix`
#define CONCAT(prefix, suffix) CONCAT_(prefix, suffix)
    /// Make a unique variable name containing the line number at the end of the
    /// name. Ex: `uint64_t MACRO_VAR(counter) = 0;` would
    /// produce `uint64_t counter_7 = 0` if the call is on line 7!
#define MACRO_VAR(prefix) CONCAT(prefix##_, __LINE__)

#define defer(start, end) for (     \
  int MACRO_VAR(_i_) = (start, 0);  \
  !MACRO_VAR(_i_);                  \
  (MACRO_VAR(_i_) += 1), end)

#define scope(end) for (            \
  int MACRO_VAR(_i_) = 0;           \
  !MACRO_VAR(_i_);                  \
  (MACRO_VAR(_i_) += 1), end)

typedef struct allocator_t allocator_t;

struct allocator_t
{
  void * user_data;
  void * (*proc)(allocator_t * this_allocator, size_t amount_to_alloc, void * ptr_to_free);
};

typedef struct {
    int age;
    char name[16];
} cat_t;

static void print_cat(void *);

static void print_cat(void * p)
{
    cat_t * cat = p;
    printf("{ age: %d, name: %s }", cat->age, cat->name);
}

typedef struct
{
  char * data;
  size_t size;
} str_t;

typedef struct
{
  char * data;
  size_t size;
  size_t cap;
  allocator_t allocator;
} str_buf_t;

static void print_str(void * p)
{
    //str_t * str = p;
    printf("TODO");
}

str_buf_t   str_buf_make(size_t size, allocator_t allocator);
void        str_buf_append(str_buf_t *, str_t);
void        str_buf_insert(str_buf_t *, str_t, size_t);
void        str_buf_remove(str_buf_t *, size_t, size_t);
void        str_buf_str(str_buf_t);

bool    str_valid               (str_t);
bool    str_match               (str_t a, str_t b);
bool    str_contains            (str_t haystack, str_t needle);
str_t   cstr                    (const char * s);
str_t   str_sub                 (str_t src, size_t begin, size_t end);
str_t   str_find_first          (str_t haystack, str_t needle);
str_t   str_find_last           (str_t haystack, str_t needle);
str_t   str_remove_prefix       (str_t src, str_t prefix);
str_t   str_remove_suffix       (str_t src, str_t suffix);
str_t   str_pop_first_split_impl(str_t * src, str_t split_by);

#define str_pop_first_split(src, split_by)                                  \
  _Generic(split_by,                                                        \
    const char *: str_pop_first_split_impl(src, cstr(split_by)),            \
    default: str_pop_first_split_impl(src, split_by))

#define for_str_split(iter, src, split_by)                                  \
    for (                                                                   \
        str_t MACRO_VAR(src_) = src,                                        \
        iter = str_pop_first_split(&MACRO_VAR(src_), split_by)              \
        MACRO_VAR(split_by_) = split_by;                                    \
        str_valid(MACRO_VAR(src_));                                         \
        iter = str_pop_first_split(&MACRO_VAR(src_), MACRO_VAR(split_by_))  \
    )

typedef void (*print_fn)(void *);

static void 
print_register_printer(const char * s, print_fn p)
{
    // TODO: store to_str in hash map keyed by s
}

static void
print(const char * fmt, ...)
{
    // TODO: iterate over fmt string, look up in hash map
    va_list ap;
    va_start(ap, fmt);
    void * p = va_arg(ap, void *);
    if (fmt[0] == '{')
        print_str(p);
    else
        print_cat(p);
    va_end(ap);
}


int main()
{
    int * p;
    static_assert(sizeof *p == 4, "sizeof int not 4");
    defer(p = malloc(sizeof *p), (free(p), p = NULL))
    {
        printf("p: %p\n", p);
    }

    scope(printf("goodbye\b"))
    {
    }


#if 0
    allocator_t temp_allocator = make_allocator(arena);
    while (game_is_running) {
        dynarr(string) strings = get_strings(temp_allocator);
        // ...
        free_temp_allocator(temp_allocator);
    }
#endif

    print_register_printer("cat", print_cat);

    cat_t c = {.name = "Mal"};
    print("Cat: {cat}", &c);

//    str_t date  = cstr("2021/03/12");
//    str_t year  = str_pop_first_split(&date, "/");
//    str_t month = str_pop_first_split(&date, "/");
//    str_t day   = str_pop_first_split(&date, "/");

    print_register_printer("str", print_str);

    str_t date = cstr("2021/03/12");
    for_str_split(it, date, "/")
    {
      print("{str}", it);
    }

    return 0;
}
