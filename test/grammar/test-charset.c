/**
 * Project Name: Project
 * Module Name: test/grammar
 * Filename: test-charset.c
 * Creator: Yaokai Liu
 * Create Date: 2024-04-04
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

//#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "xparse.h"
#include "regexp.h"
#include "inst.h"


xVoid _memcpy(xVoid *dest, const xVoid * const src, xSize size) {
    for (xSize i = 0; i < size; i ++) { ((xuByte *) dest)[i] = ((xuByte *)src)[i]; }
}

xVoid _memset(xVoid *mem, xuByte value, xSize size) {
    for (xSize i = 0; i < size; i ++) { ((xuByte *)mem)[i] = value; }
}

const static struct Allocator StdAllocator = {
        .malloc = malloc,
        .calloc = (void *(*)(xLong, xSize)) calloc,
        .free = free,
        .realloc = realloc,
        .memset = _memset,
        .memcpy = _memcpy,
};

int main() {
    xParser parser = {};
    char_t * src = string_t("abcd");
    xParser_init(&parser, &StdAllocator);
    compile_sequence(&parser, src);
    if (parser.err_len > 0) {
        printf("error@<%lld, %llu>.\n", parser.err_pos - src, parser.err_len);
        return -1;
    }
    printf("source is: \"%s\"\n", (char *) MemSpace.real_addr(parser.CHAR_SPACE, 0));

    char_t buffer[128] = {};

    xSize total_count = MemSpace.len(parser.INST_SPACE[parser.used]);
    printf("total count: %llu\n", total_count);
    for (xInt i = 0; i < total_count; i ++) {
        inst * inst = MemSpace.real_addr(parser.INST_SPACE[parser.used], i);
        inst2str(inst, buffer);
        printf("%p: %s\n", inst, buffer);
        _memset((xuByte *) buffer, 0, 128);
    }

    xParser_release(&parser);
    return 0;
}
