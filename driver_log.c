#include "driver_log.h"

void log_info(FILE* f, const char* filename, int32_t linenumber, const char* format, ...) {
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
    vfprintf(f, message, args);
    fflush(stdout);
    va_end(args);
}

void log_warn(FILE* f, const char* filename, int32_t linenumber, const char* format, ...) {
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
    vfprintf(f, message, args);
    fflush(stdout);
    va_end(args);
}

void log_fail(FILE* f, const char* filename, int32_t linenumber, const char* format, ...) {
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
    vfprintf(f, message, args);
    fflush(stdout);
    va_end(args);
}
