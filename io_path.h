#ifndef IO_PATH_H
#define IO_PATH_H

#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

#include "driver_log.h"

#define PATH_SEPERATOR '/'

bool path_file_exists(const char* path);

bool path_search_path(char* search_path, const char* filename, size_t found_path_size, char* found_path);

#endif
