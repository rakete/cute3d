#include "io_path.h"

bool path_file_exists(const char* path) {
    FILE* path_handle = fopen(path, "rb");
    if( path_handle ) {
        fclose(path_handle);
        return true;
    }

    return false;
}

bool path_search_path(char* shader_search_path, const char* filename, size_t found_path_size, char* found_path) {
    // - this function takes a shader_search_path like "directory1/:directory2/" and searches through them for
    // filename, if filename is found in one of the directories the full path like "directory2/filename" is
    // copied into found_path, which is allocated externally with size found_path_size
    log_assert( shader_search_path != NULL );
    log_assert( filename != NULL );
    log_assert( found_path != NULL );

    // - we'll use the size of the whole shader_search_path later to allocate enough memory for tokens and we'll
    // need when we want to point to the end of the shader_search_path, so we get it here
    // - also should be at least one character long otherwise the token_length below becomes zero and things go
    // pear shaped
    size_t shader_search_path_size = strlen(shader_search_path);
    log_assert( shader_search_path_size > 1 );

    // - found_path_size must be at least one two, so we can null terminate the found_path right at the beginning
    log_assert( found_path_size > 1 );
    for( size_t i = 0; i < found_path_size; i++ ) {
        found_path[i] = '\0';
    }

    // - time to allocate some memory for our tokens, a token is one directory string between colons from the
    // shader_search_path, so in "dir1:dir2:dir3:" the dir1, dir2, dir3 strings are tokens
    // - by using shader_search_path_size + strlen(filename) + 2 as size for the allocation we use vastly too much
    // memory, but properly still a miniscule amount so I am fine with it because it guarantees that we have
    // memory for every token + a potential filename + a directory seperator + \0
    size_t token_buf_alloc = shader_search_path_size + strlen(filename) + 2;
    char* token_buf = malloc(token_buf_alloc);
    log_assert( token_buf != NULL );

    // - before the fun ride starts we need some state, first three flags which indicate that:
    // - we searched every directory from shader_search_path for filename and are now finished, this does not
    // indicate success
    bool search_finished = false;
    // - we successfully found filename in a directory from shader_search_path
    bool search_success = false;
    // - the shader_search_path contains only one directory, meaning we don't have to loop and try searching in
    // another directory because there is none
    bool search_singular = false;
    // - token_begin is a pointer into the shader_search_path, pointing at the begin of a directory token, this
    // means it should be pointing at the first character of a directory right after a colon seperator
    // - there may be leading colons at the beginning of shader_search_path, like so ":::dir1:dir2", the while
    // loop below skips over any leading colons
    char* token_begin = shader_search_path;
    while( token_begin[0] == ':' ) {
        token_begin += 1;
    }
    // - token_end is a pointer into the shader_search_path, pointing at the end of a directory token, this
    // means it should be pointing at the character that comes just after the last character of a directory token,
    // so it would point to the colon seperator that comes before the next token, or the nul terminating character
    // at the end of shader_search_path
    // - therefore token_end is initialized by searching shader_search_path from token_begin to where the first
    // colon appears
    // - if no colon is found token_end will be NULL, then that means the shader_search_path contains only one
    // directory, so we set token_end to point to the end of shader_search_path and set search_singular to true
    char* token_end = strchr(token_begin, ':');
    if( token_end == NULL ) {
        token_end = &shader_search_path[shader_search_path_size];
        search_singular = true;
    }

    // - now the fun part starts, a while loop that copies the string between token_begin and token_end as
    // directory name, concatenates it with the filename, checks if the resulting path exists and breaks if
    // it does or continues with the next token otherwise by moving token_begin and token_end along the
    // shader_search_path
    while( search_finished == false ) {
        // - this check is not relevant for the first iteration, its only relevant for any nth iteration
        // after the first
        // - if the strchr at the end of this loop does not find a colon, and sets token_end to NULL, then
        // we set it to point to the end of shader_search_path here, so that we then can do one more iteration
        // to check the last token
        // - we do this check here and not below the strchr at the end of the while loop because we need
        // to set search_finished to true, and if we did that in the last line, the last iteration to check
        // the last token would not run
        if( token_end == NULL ) {
            token_end = &shader_search_path[shader_search_path_size];
            search_finished = true;
        }

        // - token_length is just the lengtt of a token in characters, which I assert to be > 0 because I
        // skip over multiple successive : when setting token_begin, so token_length should be impossible
        // to be 0 here
        intptr_t token_length = token_end - token_begin;
        log_assert( token_length > 0 );

        // - copy the directory string from shader_search_path into token_buf and nul terminate it
        memcpy(token_buf, token_begin, token_length);
        token_buf[token_length] = '\0';

        // - concatenate token_buf with filename into found_path
        // - only use a seperator if neither the last char of token_buf or the first char of filename
        // is a seperator
        if( token_buf[token_length-1] == PATH_SEPERATOR || filename[0] == PATH_SEPERATOR ) {
            snprintf(found_path, found_path_size, "%s%s", token_buf, filename);
        } else {
            snprintf(found_path, found_path_size, "%s%c%s", token_buf, PATH_SEPERATOR, filename);
        }

        // - test the concatenated path exists in the filesystem, if it does we were succesful and can break
        if( path_file_exists(found_path) ) {
            search_success = true;
            break;
        }

        // - if shader_search_path contains only one directory to search, and we are here, then the search were
        // unsuccessful and we can break
        if( search_singular == true ) {
            break;
        }


        // - move token_begin to the beginning of the next token by increasing token_end, and skipping over any multiple
        // occurences of colons should there be any
        token_begin = token_end+1;
        while( token_begin[0] == ':' ) {
            token_begin += 1;
        }
        // - then move token_end the next token by searching for a colon with strchr, starting from the new token_begin,
        // should strchr return NULL because no colon is found, the code above at the begin of the while loop runs and
        // sets token_end to point to the end of the shader_search_path
        token_end = strchr(token_begin, ':');
    }

    free(token_buf);

    return search_success;
}
