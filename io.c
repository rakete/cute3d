#include "io.h"

char* read_file(const char* filename, int* length) {
    FILE* file = fopen(filename, "rb");

    if( ! file ) {
        char* path = malloc(strlen(ENGINE_ROOT) + strlen(filename) + 1);
        sprintf(path, "%s%s\0", ENGINE_ROOT, filename);
        file = fopen(path, "rb");
    }

    if( ! file ) {
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    *length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* contents = malloc(*length);
    fread(contents, *length, 1, file);
    fclose(file);
    
    return contents;
}
