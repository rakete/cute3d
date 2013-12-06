#include "io.h"

int main(int argc, char *argv[]) {
    unsigned char input[1024];
    for( int i = 0; i < 1024; i++ ) {
        input[i] = 99;
    }
    input[1023] = 100;

    uint64_t input2[2] = { 1,2 };
    unsigned char* output = NULL;

    size_t allocated = rle_encode((unsigned char*)input2, sizeof(input2), 1, 0xaa, &output);

    for( int i = 0; i < allocated; i++ ) {
        printf("%d ", output[i]);
    }
    printf("\n%lu %lu\n", sizeof(input2), allocated);

    return 0;
}
