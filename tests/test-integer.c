#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"
#include "limits.h"

#include "stdio.h"

int main(int argc, char *argv[]) {
    printf("type           : size  min/max\n");
    printf("----------------------------------------------------------------\n");
    printf("char           :    %zu, %d\n", sizeof(char), CHAR_MIN);
    printf("unsigned char  :    %zu,  %u\n", sizeof(unsigned char), UCHAR_MAX);
    printf("wchar_t        :    %zu,  %d\n", sizeof(wchar_t), WCHAR_MAX);
    printf("short          :    %zu, %d\n", sizeof(short), SHRT_MIN);
    printf("unsigned short :    %zu,  %u\n", sizeof(unsigned short), USHRT_MAX);
    printf("int            :    %zu, %d\n", sizeof(int), INT_MIN);
    printf("unsigned int   :    %zu,  %u\n", sizeof(unsigned int), UINT_MAX);
    printf("long           :    %zu, %ld\n", sizeof(long), LONG_MIN);
    printf("unsigned long  :    %zu,  %lu\n", sizeof(unsigned long), ULONG_MAX);
    printf("\n");


    printf("int8_t         :    %zu, %d\n", sizeof(int8_t), INT8_MIN);
    printf("uint8_t        :    %zu,  %u\n", sizeof(uint8_t), UINT8_MAX);
    printf("int_least8_t   :    %zu, %d\n", sizeof(int_least8_t), INT_LEAST8_MIN);
    printf("uint_least8_t  :    %zu,  %u\n", sizeof(uint_least8_t), UINT_LEAST8_MAX);
    printf("int_fast8_t    :    %zu, %d\n", sizeof(int_fast8_t), INT_FAST8_MIN);
    printf("uint_fast8_t   :    %zu,  %u\n", sizeof(uint_fast8_t), UINT_FAST8_MAX);
    printf("\n");

    printf("int16_t        :    %zu, %d\n", sizeof(int16_t), INT16_MIN);
    printf("uint16_t       :    %zu,  %u\n", sizeof(uint16_t), UINT16_MAX);
    printf("int_least16_t  :    %zu, %d\n", sizeof(int_least16_t), INT_LEAST16_MIN);
    printf("uint_least16_t :    %zu,  %u\n", sizeof(uint_least16_t), UINT_LEAST16_MAX);
    printf("int_fast16_t   :    %zu, %d\n", sizeof(int_fast16_t), INT_FAST16_MIN);
    printf("uint_fast16_t  :    %zu,  %u\n", sizeof(uint_fast16_t), UINT_FAST16_MAX);
    printf("\n");

    printf("int32_t        :    %zu, %d\n", sizeof(int32_t), INT32_MIN);
    printf("uint32_t       :    %zu,  %u\n", sizeof(uint32_t), UINT32_MAX);
    printf("int_least32_t  :    %zu, %d\n", sizeof(int_least32_t), INT_LEAST32_MIN);
    printf("uint_least32_t :    %zu,  %u\n", sizeof(uint_least32_t), UINT_LEAST32_MAX);
    printf("int_fast32_t   :    %zu, %d\n", sizeof(int_fast32_t), INT_FAST32_MIN);
    printf("uint_fast32_t  :    %zu,  %u\n", sizeof(uint_fast32_t), UINT_FAST32_MAX);
    printf("\n");

    printf("int64_t        :    %zu, %lld\n", sizeof(int64_t), INT64_MIN);
    printf("uint64_t       :    %zu,  %llu\n", sizeof(uint64_t), UINT64_MAX);
    printf("int_least64_t  :    %zu, %lld\n", sizeof(int_least64_t), INT_LEAST64_MIN);
    printf("uint_least64_t :    %zu,  %llu\n", sizeof(uint_least64_t), UINT_LEAST64_MAX);
    printf("int_fast64_t   :    %zu, %lld\n", sizeof(int_fast64_t), INT_FAST64_MIN);
    printf("uint_fast64_t  :    %zu,  %llu\n", sizeof(uint_fast64_t), UINT_FAST64_MAX);
    printf("\n");

    printf("intptr_t       :    %zu, %d\n", sizeof(intptr_t), INTPTR_MIN);
    printf("uintptr_t      :    %zu,  %u\n", sizeof(uintptr_t), UINTPTR_MAX);

    printf("intmax_t       :    %zu, %lld\n", sizeof(intmax_t), INTMAX_MIN);
    printf("uintmax_t      :    %zu,  %llu\n", sizeof(uintmax_t), UINTMAX_MAX);

    printf("ptrdiff_t      :    %zu, %d\n", sizeof(ptrdiff_t), PTRDIFF_MIN);
    printf("size_t         :    %zu,  %u\n", sizeof(size_t), SIZE_MAX);
    printf("\n");

    printf("bool           :    %zu, true\n", sizeof(bool));
    printf("\n");

    return 0;
}
