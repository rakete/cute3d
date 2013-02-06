#ifndef IO_H
#define IO_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"

#define ENGINE_ROOT "/home/rakete/interactive/cute3d/"

char* read_file(const char* filename, uint32_t* length);

#endif
