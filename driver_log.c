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

#include "driver_log.h"

int32_t global_log_indent_level = 0;

void log_indent(int32_t level) {
    global_log_indent_level += level*LOG_INDENT_TABWIDTH;
    if( global_log_indent_level < 0 ) {
        global_log_indent_level = 0;
    } else if( global_log_indent_level > MAX_LOG_INDENT ) {
        global_log_indent_level = MAX_LOG_INDENT;
    }
}

void log_printf(const char* message, va_list args) {
    for( int32_t i = 0; i < global_log_indent_level; i++ ) {
        fprintf(stdout, "%c", LOG_INDENT_CHAR);
    }
    vfprintf(stdout, message, args);
    fflush(stdout);
}

void log_message(const char* color, const char* tag, const char* filename, int32_t linenumber, const char* format, size_t n, char* message) {
    if( filename && linenumber > 0 ) {
#ifdef DEBUG
        char indentation[MAX_LOG_INDENT+1];
        for( int32_t i = 0; i < global_log_indent_level; i++ ) {
            indentation[i] = LOG_INDENT_CHAR;
        }
        indentation[global_log_indent_level] = '\0';
        snprintf(message, n, "%s%s" LOG_ANSI_COLOR_RESET "%s" "%s%s" LOG_ANSI_COLOR_RESET "%s:%d:\n", color, tag, format, indentation, color, filename, linenumber);
#else
        snprintf(message, n, "%s%s" LOG_ANSI_COLOR_RESET "%s", color, tag, format);
#endif
    } else {
        snprintf(message, n, "%s%s" LOG_ANSI_COLOR_RESET "%s", color, tag, format);
    }
}

void log_info(const char* filename, int32_t linenumber, const char* format, ...) {
    va_list args;
    va_start(args, format);

    char message[8192];
    log_message(LOG_ANSI_COLOR_GREEN, "[INFO] ", filename, linenumber, format, 8192, message);

    log_printf(message, args);
    va_end(args);
}

void log_warn(const char* filename, int32_t linenumber, const char* format, ...) {
    va_list args;
    va_start(args, format);

    char message[8192];
    log_message(LOG_ANSI_COLOR_BLUE, "[WARN] ", filename, linenumber, format, 8192, message);

    log_printf(message, args);
    va_end(args);
}

void log_fail(const char* filename, int32_t linenumber, const char* format, ...) {
    va_list args;
    va_start(args, format);

    char message[8192];
    log_message(LOG_ANSI_COLOR_RED, "[FAIL] ", filename, linenumber, format, 8192, message);

    log_printf(message, args);
    va_end(args);
}

void log_continue(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_printf(format, args);
    va_end(args);
}

bool _log_assert(const char* condition, const char* format, ...) {
    va_list args;
    va_start(args, format);

    printf("\n" LOG_ANSI_COLOR_RED "[FAIL]" LOG_ANSI_COLOR_RESET " assert( %s );\n", condition);
    if( strlen(format) ) {
        printf(LOG_ANSI_COLOR_BLUE);
        log_printf(format, args);
        printf(LOG_ANSI_COLOR_RESET);
        fflush(stdout);
    }

    va_end(args);
    return false;
}
