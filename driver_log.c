#include "driver_log.h"

bool log_info(const char* filename, int32_t linenumber, const char* format, ...) {
    va_list args;
    va_start(args, format);

    char message[8192];
    if( filename ) {
#ifdef DEBUG
        snprintf(message, 8192, "%s:%d::" LOG_ANSI_COLOR_GREEN " [INFO] " LOG_ANSI_COLOR_RESET "%s", filename, linenumber, format);
#else
        snprintf(message, 8192, LOG_ANSI_COLOR_GREEN "[INFO] " LOG_ANSI_COLOR_RESET "%s", format);
#endif
    } else {
        snprintf(message, 8192, LOG_ANSI_COLOR_GREEN "[INFO] " LOG_ANSI_COLOR_RESET "%s", format);
    }
    vfprintf(stdout, message, args);
    fflush(stdout);
    va_end(args);

    return true;
}

bool log_warn(const char* filename, int32_t linenumber, const char* format, ...) {
    va_list args;
    va_start(args, format);

    char message[8192];
    if( filename ) {
#ifdef DEBUG
        snprintf(message, 8192, "%s:%d::" LOG_ANSI_COLOR_BLUE " [WARN] " LOG_ANSI_COLOR_RESET "%s", filename, linenumber, format);
#else
        snprintf(message, 8192, LOG_ANSI_COLOR_BLUE "[WARN] " LOG_ANSI_COLOR_RESET "%s", format);
#endif
    } else {
        snprintf(message, 8192, LOG_ANSI_COLOR_BLUE "[WARN] " LOG_ANSI_COLOR_RESET "%s", format);
    }
    vfprintf(stdout, message, args);
    fflush(stdout);
    va_end(args);

    return true;
}

bool log_fail(const char* filename, int32_t linenumber, const char* format, ...) {
    va_list args;
    va_start(args, format);

    char message[8192];
    if( filename ) {
#ifdef DEBUG
        snprintf(message, 8192, "%s:%d::" LOG_ANSI_COLOR_RED " [FAIL] " LOG_ANSI_COLOR_RESET "%s", filename, linenumber, format);
#else
        snprintf(message, 8192, LOG_ANSI_COLOR_RED "[FAIL] " LOG_ANSI_COLOR_RESET "%s", format);
#endif
    } else {
        snprintf(message, 8192, LOG_ANSI_COLOR_RED "[FAIL] " LOG_ANSI_COLOR_RESET "%s", format);
    }
    vfprintf(stdout, message, args);
    fflush(stdout);
    va_end(args);

    return true;
}

bool log_continue(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    fflush(stdout);

    return true;
}

bool _log_assert(const char* condition, const char* format, ...) {
    va_list args;
    va_start(args, format);
    va_end(args);

    printf("\n" LOG_ANSI_COLOR_RED "[FAIL]" LOG_ANSI_COLOR_RESET " assert( %s );\n", condition);
    if( strlen(format) ) {
        printf(LOG_ANSI_COLOR_BLUE);
        vfprintf(stdout, format, args);
        printf(LOG_ANSI_COLOR_RESET);
        fflush(stdout);
    }

    return false;
}
