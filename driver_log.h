#ifndef DRIVER_LOG_H
#define DRIVER_LOG_H

#include "assert.h"
#include "stdio.h"
#include "stdarg.h"
#include "stdint.h"

#define LOG_ANSI_COLOR_RED     "\x1b[31m"
#define LOG_ANSI_COLOR_GREEN   "\x1b[32m"
#define LOG_ANSI_COLOR_YELLOW  "\x1b[33m"
#define LOG_ANSI_COLOR_BLUE    "\x1b[34m"
#define LOG_ANSI_COLOR_MAGENTA "\x1b[35m"
#define LOG_ANSI_COLOR_CYAN    "\x1b[36m"
#define LOG_ANSI_COLOR_RESET   "\x1b[0m"

void log_info(FILE* f, const char* filename, int32_t linenumber, const char* format, ...);
void log_warn(FILE* f, const char* filename, int32_t linenumber, const char* format, ...);
void log_fail(FILE* f, const char* filename, int32_t linenumber, const char* format, ...);

#endif
