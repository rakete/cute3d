#include "io_path.h"

bool path_file_exists(const char* path) {
    FILE* path_handle = fopen(path, "rb");
    if( path_handle ) {
        fclose(path_handle);
        return true;
    }

    return false;
}

bool path_search_path(char* search_path, const char* filename, size_t found_path_size, char* found_path) {
    size_t search_path_size = strlen(search_path);
    log_assert( search_path_size > 1 );

    log_assert( found_path_size > 1 );
    for( size_t i = 0; i < found_path_size; i++ ) {
        found_path[i] = '\0';
    }

    size_t path_str_alloc = search_path_size + strlen(filename);
    char* path_str = malloc(path_str_alloc);

    bool path_search_finished = false;
    bool path_search_success = false;
    bool path_search_singular = false;

    char* path_begin = search_path;
    while( path_begin[0] == ':' ) {
        path_begin += 1;
    }
    char* path_end = strchr(path_begin, ':');
    if( path_end == NULL ) {
        path_end = &search_path[search_path_size];
        path_search_singular = true;
    }
    size_t path_length = path_end - path_begin;

    while( path_search_finished == false ) {
        if( path_end == NULL ) {
            path_end = &search_path[search_path_size];
            path_search_finished = true;
        }

        path_length = path_end - path_begin;
        if( path_length == 0 ) {
            break;
        }

        memcpy(path_str, path_begin, path_length);
        path_str[path_length] = '\0';

        if( path_str[path_length-1] == PATH_SEPERATOR || filename[0] == PATH_SEPERATOR ) {
            snprintf(found_path, found_path_size, "%s%s", path_str, filename);
        } else {
            snprintf(found_path, found_path_size, "%s%c%s", path_str, PATH_SEPERATOR, filename);
        }

        if( path_file_exists(found_path) ) {
            path_search_success = true;
            break;
        }

        if( path_search_singular == true ) {
            break;
        }

        path_begin = path_end+1;
        path_end = strchr(path_begin, ':');
    }

    free(path_str);

    return path_search_success;
}
