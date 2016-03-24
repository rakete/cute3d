#ifndef DRIVER_LOG_H
#define DRIVER_LOG_H

#include "assert.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "stdint.h"
#include "stdbool.h"

#define LOG_ANSI_COLOR_RED     "\x1b[31m"
#define LOG_ANSI_COLOR_GREEN   "\x1b[32m"
#define LOG_ANSI_COLOR_YELLOW  "\x1b[33m"
#define LOG_ANSI_COLOR_BLUE    "\x1b[34m"
#define LOG_ANSI_COLOR_MAGENTA "\x1b[35m"
#define LOG_ANSI_COLOR_CYAN    "\x1b[36m"
#define LOG_ANSI_COLOR_RESET   "\x1b[0m"

#define log_stringify(a) #a
#define log_tostring(x) log_stringify(x)

bool log_info(FILE* f, const char* filename, int32_t linenumber, const char* format, ...);
bool log_warn(FILE* f, const char* filename, int32_t linenumber, const char* format, ...);
bool log_fail(FILE* f, const char* filename, int32_t linenumber, const char* format, ...);

bool log_continue(FILE* f, const char* format, ...);

// - logging assert failures, the log_assert macro takes can be used with just one argument, the asserted condtion,
// and optionally a format string and variadic args can come after the condition
// - I had to implement it with these multiple macros and extra "", "" arguments to work around warnings (also I could
// not implement it without the "", "" because C99 requires that the ... gets at least one argument)
// - I discovered an interesting trick that I didn't use: there can be a macro #define foo() and function definition
// void (foo)() both in the same program without problems, the macro will be used whenever foo() is used, but when written
// like this (foo)(), the function will be used
#define log_assert(...) _log_assert_dispatch(__VA_ARGS__, "", "");
#define _log_assert_dispatch(condition, format, ...) do { assert((condition) || _log_assert(#condition, format, __VA_ARGS__)); } while(0)
bool _log_assert(const char* condition, const char* format, ...);

/* _Pragma(log_stringify(clang diagnostic push));                      \ */
/* _Pragma(log_stringify(clang diagnostic ignored "-Wformat-zero-length")) \ */
/* _Pragma(log_stringify(clang diagnostic ignored "-Wformat-extra-args")) \ */

/* #define _log_assert(condition, format, ...)                           \ */
/*     _Pragma(log_stringify(GCC diagnostic push));                        \ */
/*     _Pragma(log_stringify(GCC diagnostic ignored "-Wformat-zero-length")) \ */
/*     _Pragma(log_stringify(GCC diagnostic ignored "-Wformat-extra-args")) \ */
/*     if( condition ) {} else { printf(format, __VA_ARGS__); }            \ */
/*     assert(condition);                                                  \ */
/*     _Pragma(log_stringify(GCC diagnostic pop)); */

#endif
