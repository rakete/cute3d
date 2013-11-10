/* cute3d, a simplistic opengl based engine written in C */
/* Copyright (C) 2013 Andreas Raster */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "io.h"

char* read_file(const char* filename, int32_t* length) {
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

static void encode_packet(unsigned char* input,
                          size_t input_offset,
                          size_t bytes,
                          unsigned char flag,
                          uint64_t* literal_run,
                          uint64_t* encoded_run,
                          uint64_t* allocated,
                          unsigned char** result)
{
    uint64_t encoded_begin = input_offset - (*encoded_run)*bytes;
    uint64_t literal_begin = encoded_begin - (*literal_run)*bytes;
    uint64_t literal_end = encoded_begin;

    size_t alloc_offset = *allocated + *literal_run * bytes;
    uint64_t output_offset = alloc_offset;
    if( *encoded_run > 1 ) {
        alloc_offset += 3 * bytes;
    }
    unsigned char* output = realloc(*result, alloc_offset);
    //printf("%lu %lu %lu %lu %lu %lu\n", encoded_begin, input_offset, *allocated, *literal_run, *encoded_run, alloc_offset);
    
    for( uint64_t l = literal_begin; l < literal_end ; l+=bytes ) {
        for( int b = 0; b < bytes; b++ ) {
            output[*allocated+b] = input[l+b];
        }
    }
    *literal_run = 0;

    while( *encoded_run > 1 ) {
        output[output_offset+0*bytes] = flag;

        if( bytes == 1 && *encoded_run > 255 ) {
            output[output_offset+1*bytes] = 255;
            *encoded_run -= 255;
            
            alloc_offset += 3;
            output = realloc(*result, alloc_offset);
            
            output_offset += 3;
        } else if( bytes == 2 && *encoded_run > 65535 ) {
            //printf("%d", 65535);
            *encoded_run -= 65535;
        } else if( bytes == 4 && *encoded_run > 4294967295 ) {
            //printf("%lu", 4294967295);
            *encoded_run -= 4294967295; 
        } else {
            for( int b = 0; b < bytes; b++ ) {
                output[output_offset+1*bytes+b] = ((unsigned char*)encoded_run)[b];
            }
            *encoded_run = 1;
        }
            
        for( int b = 0; b < bytes; b++ ) {
            output[output_offset+2*bytes+b] = input[encoded_begin+b];
        }
    }

    *result = output;
    *allocated = alloc_offset;
}

size_t rle_encode(unsigned char* input, size_t size, size_t bytes, unsigned char flag, unsigned char** output) {
    if( bytes != 1 && bytes != 2 && bytes != 4 && bytes != 8 ) {
        return 0;
    }
    
    unsigned char last[bytes];
    uint64_t literal_run = 0;
    uint64_t encoded_run = 0;
    size_t allocated = 0;
    int force = 0;
    
    for( size_t i = 0; i < size; i+=bytes ) {
        unsigned char current[bytes];
        int equal = 1;

        for( int b = 0; b < bytes; b++ ) {
            current[b] = input[i+b];
            if( i > 0 && current[b] != last[b] ) {
                equal = 0;
            }
        }

        if( i == 0 || equal ) {
            encoded_run += 1;
        } else if( ! equal && ! force && encoded_run <= 3 ) {
            literal_run += encoded_run;
            encoded_run = 1;
        } else if( ! equal && encoded_run > 3 || force ) {
            //printf("-> %lu %lu", literal_run, encoded_run);
            encode_packet(input, i, bytes, flag, &literal_run, &encoded_run, &allocated, output);
        }

        for( int b = 0; b < bytes; b++ ) {
            last[b] = current[b];
        }

        if( input[i] == flag ) {
            force = 1;
        } else {
            force = 0;
        }
    }
    if( encoded_run <= 3 ) {
        literal_run += encoded_run;
        encoded_run = 0;
    }
    //printf("-> %lu %lu\n", literal_run, encoded_run);
    encode_packet(input, size, bytes, flag, &literal_run, &encoded_run, &allocated, output);
    
    // literal_run
    // encoded_run
    return allocated;
}
