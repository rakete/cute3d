/* Cute3D, a simple opengl based framework for writing interactive realtime applications */

/* Copyright (C) 2013-2017 Andreas Raster */

/* This file is part of Cute3D. */

/* Cute3D is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* Cute3D is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with Cute3D.  If not, see <http://www.gnu.org/licenses/>. */

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

void log_printf(const char* message, va_list args);
void log_message(const char* color, const char* tag, const char* filename, int32_t linenumber, const char* format, size_t n, char* message);

void log_info(const char* filename, int32_t linenumber, const char* format, ...);
void log_warn(const char* filename, int32_t linenumber, const char* format, ...);
void log_fail(const char* filename, int32_t linenumber, const char* format, ...);
void log_continue(const char* format, ...);

#ifdef CUTE_BUILD_MSVC
// http://stackoverflow.com/questions/9183993/msvc-variadic-macro-expansion
// http://stackoverflow.com/questions/24836793/varargs-elem-macro-for-use-with-c/24837037#24837037
#define _log_glue(x, y) x y

#define _log_return_arg_count(_1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, count, ...) count
#define _log_expand_args(args) _log_return_arg_count args
#define _log_count_args_max16(...) _log_expand_args((__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#define _log_overload_macro2(name, count) name##count
#define _log_overload_macro1(name, count) _log_overload_macro2(name, count)
#define _log_overload_macro(name, count) _log_overload_macro1(name, count)

#define _log_call_overload(name, ...) _log_glue(_log_overload_macro(name, _log_count_args_max16(__VA_ARGS__)), (__VA_ARGS__))

#define _log_assert_dispatch0(condition)
#define _log_assert_dispatch1(condition) do { assert((condition) || _log_assert(#condition, "")); } while(0)
#define _log_assert_dispatch2(condition, format) do { assert((condition) || _log_assert(#condition, format)); } while(0)
#define _log_assert_dispatch3(condition, format, a) do { assert((condition) || _log_assert(#condition, format, a)); } while(0)
#define _log_assert_dispatch4(condition, format, a, b) do { assert((condition) || _log_assert(#condition, format, a, b)); } while(0)
#define _log_assert_dispatch5(condition, format, a, b, c) do { assert((condition) || _log_assert(#condition, format, a, b, c)); } while(0)
#define _log_assert_dispatch6(condition, format, a, b, c, d) do { assert((condition) || _log_assert(#condition, format, a, b, c, d)); } while(0)
#define _log_assert_dispatch7(condition, format, a, b, c, d, e) do { assert((condition) || _log_assert(#condition, format, a, b, c, d, e)); } while(0)
#define _log_assert_dispatch8(condition, format, a, b, c, d, e, f) do { assert((condition) || _log_assert(#condition, format, a, b, c, d, e, f)); } while(0)
#define _log_assert_dispatch9(condition, format, a, b, c, d, e, f, g) do { assert((condition) || _log_assert(#condition, format, a, b, c, d, e, f, g)); } while(0)
#define _log_assert_dispatch10(condition, format, a, b, c, d, e, f, g, h) do { assert((condition) || _log_assert(#condition, format, a, b, c, d, e, f, g, h)); } while(0)
#define _log_assert_dispatch11(condition, format, a, b, c, d, e, f, g, h, i) do { assert((condition) || _log_assert(#condition, format, a, b, c, d, e, f, g, h, i)); } while(0)
#define _log_assert_dispatch12(condition, format, a, b, c, d, e, f, g, h, i, j) do { assert((condition) || _log_assert(#condition, format, a, b, c, d, e, f, g, h, i, j)); } while(0)
#define _log_assert_dispatch13(condition, format, a, b, c, d, e, f, g, h, i, j, k) do { assert((condition) || _log_assert(#condition, format, a, b, c, d, e, f, g, h, i, j, k)); } while(0)
#define _log_assert_dispatch14(condition, format, a, b, c, d, e, f, g, h, i, j, k, l) do { assert((condition) || _log_assert(#condition, format, a, b, c, d, e, f, g, h, i, j, k, l)); } while(0)
#define _log_assert_dispatch15(condition, format, a, b, c, d, e, f, g, h, i, j, k, l, m) do { assert((condition) || _log_assert(#condition, format, a, b, c, d, e, f, g, h, i, j, k, l, m)); } while(0)
#define _log_assert_dispatch16(condition, format, a, b, c, d, e, f, g, h, i, j, k, l, m, n) do { assert((condition) || _log_assert(#condition, format, a, b, c, d, e, f, g, h, i, j, k, l, m, n)); } while(0)

#define log_assert(...) _log_call_overload(_log_assert_dispatch, __VA_ARGS__);

#else

// - logging assert failures, the log_assert macro takes can be used with just one argument, the asserted condtion,
// and optionally a format string and variadic args can come after the condition
// - I had to implement it with these multiple macros and extra "", "" arguments to work around warnings (also I could
// not implement it without the "", "" because C99 requires that the ... gets at least one argument)
// - I discovered an interesting trick that I didn't use: there can be a macro #define foo() and function definition
// void (foo)() both in the same program without problems, the macro will be used whenever foo() is used, but when written
// like this (foo)(), the function will be used
#define log_assert(...) _log_assert_dispatch(__VA_ARGS__, "", "");
#define _log_assert_dispatch(condition, format, ...) do { assert((condition) || _log_assert(#condition, format, __VA_ARGS__)); } while(0)

#endif
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

#define alloc_vla(t, name, n) t name[n]; do {                           \
        size_t size = sizeof(t) * n;                                    \
        if( size > 30000 ) {                                            \
            log_warn(__FILE__, __LINE__, "%lu bytes stack allocation\n", size); \
        }                                                               \
    } while(0);

#endif
