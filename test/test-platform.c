/**
 * Project Name: xParse
 * Module Name: test
 * Filename: main.c
 * Creator: Yaokai Liu
 * Create Date: 2024-03-27
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include <stdlib.h>
#include "vm.h"
#include "inst.h"
#include "allocator.h"
#include <stdio.h>

const static struct Allocator StdAllocator = {
        .malloc = malloc,
        .calloc = (void *(*)(xLong, xSize)) calloc,
        .free = free,
        .realloc = realloc,
        .memset = nullptr,
        .memcpy = nullptr,
};

int main() {
    xInt offset = -123987432;
    xuLong val1 = 12312312311231;
    printf("%016lx, %016lx\n", val1, val1 + offset);
    printf("val1 > val1 + offset: %b\n", val1 > val1 + offset);

    xuLong L = 0xffff'ffff'ffff'0000;
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    printf("%u\n", ((xuShort *) &L)[3]);
#elif (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    printf("%u\n", ((xuShort *) &L)[0]);
#endif
};
