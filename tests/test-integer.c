#include "stdint.h"
#include "stddef.h"
#include "limits.h"

#include "stdio.h"

int main(int argc, char *argv[]) {
    printf("type           : size  min/max\n");
    printf("----------------------------------------------------------------\n");
    printf("char           :    %lu, %d\n", sizeof(char), CHAR_MIN);
    printf("unsigned char  :    %lu,  %u\n", sizeof(unsigned char), UCHAR_MAX);
    printf("short          :    %lu, %d\n", sizeof(short), SHRT_MIN);
    printf("unsigned short :    %lu,  %u\n", sizeof(unsigned short), USHRT_MAX);
    printf("int            :    %lu, %d\n", sizeof(int), INT_MIN);
    printf("unsigned int   :    %lu,  %u\n", sizeof(unsigned int), UINT_MAX);
    printf("long           :    %lu, %ld\n", sizeof(long), LONG_MIN);
    printf("unsigned long  :    %lu,  %lu\n", sizeof(unsigned long), ULONG_MAX);
    printf("\n");

    printf("int8_t         :    %lu, %d\n", sizeof(int8_t), INT8_MIN);
    printf("uint8_t        :    %lu,  %u\n", sizeof(uint8_t), UINT8_MAX);
    printf("int_least8_t   :    %lu, %d\n", sizeof(int_least8_t), INT_LEAST8_MIN);
    printf("uint_least8_t  :    %lu,  %u\n", sizeof(uint_least8_t), UINT_LEAST8_MAX);
    printf("int_fast8_t    :    %lu, %d\n", sizeof(int_fast8_t), INT_FAST8_MIN);
    printf("uint_fast8_t   :    %lu,  %u\n", sizeof(uint_fast8_t), UINT_FAST8_MAX);
    printf("\n");

    printf("int16_t        :    %lu, %d\n", sizeof(int16_t), INT16_MIN);
    printf("uint16_t       :    %lu,  %u\n", sizeof(uint16_t), UINT16_MAX);
    printf("int_least16_t  :    %lu, %d\n", sizeof(int_least16_t), INT_LEAST16_MIN);
    printf("uint_least16_t :    %lu,  %u\n", sizeof(uint_least16_t), UINT_LEAST16_MAX);
    printf("int_fast16_t   :    %lu, %ld\n", sizeof(int_fast16_t), INT_FAST16_MIN);
    printf("uint_fast16_t  :    %lu,  %lu\n", sizeof(uint_fast16_t), UINT_FAST16_MAX);
    printf("\n");

    printf("int32_t        :    %lu, %d\n", sizeof(int32_t), INT32_MIN);
    printf("uint32_t       :    %lu,  %u\n", sizeof(uint32_t), UINT32_MAX);
    printf("int_least32_t  :    %lu, %d\n", sizeof(int_least32_t), INT_LEAST32_MIN);
    printf("uint_least32_t :    %lu,  %u\n", sizeof(uint_least32_t), UINT_LEAST32_MAX);
    printf("int_fast32_t   :    %lu, %ld\n", sizeof(int_fast32_t), INT_FAST32_MIN);
    printf("uint_fast32_t  :    %lu,  %lu\n", sizeof(uint_fast32_t), UINT_FAST32_MAX);
    printf("\n");

    printf("int64_t        :    %lu, %ld\n", sizeof(int64_t), INT64_MIN);
    printf("uint64_t       :    %lu,  %lu\n", sizeof(uint64_t), UINT64_MAX);
    printf("int_least64_t  :    %lu, %ld\n", sizeof(int_least64_t), INT_LEAST64_MIN);
    printf("uint_least64_t :    %lu,  %lu\n", sizeof(uint_least64_t), UINT_LEAST64_MAX);
    printf("int_fast64_t   :    %lu, %ld\n", sizeof(int_fast64_t), INT_FAST64_MIN);
    printf("uint_fast64_t  :    %lu,  %lu\n", sizeof(uint_fast64_t), UINT_FAST64_MAX);
    printf("\n");

    printf("intptr_t       :    %lu, %ld\n", sizeof(intptr_t), INTPTR_MIN);
    printf("uintptr_t      :    %lu,  %lu\n", sizeof(uintptr_t), UINTPTR_MAX);

    printf("intmax_t       :    %lu, %ld\n", sizeof(intmax_t), INTMAX_MIN);
    printf("uintmax_t      :    %lu,  %lu\n", sizeof(uintmax_t), UINTMAX_MAX);

    printf("ptrdiff_t      :    %lu, %ld\n", sizeof(ptrdiff_t), PTRDIFF_MIN);
    printf("size_t         :    %lu,  %lu\n", sizeof(size_t), SIZE_MAX);
    printf("\n");

    return 0;
}
