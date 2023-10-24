//
// Project Name: xParse
// Filename: begin.c
// Creator: Yaokai Liu
// Create Date: 2023-10-23
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//
#include "regexp.h"
#include "symbols.h"
#include "string.h"
#include "codegen.h"

xSize compile_begin(xParser * parser, char_t * regexp) {
    char_t *sp = regexp;

    if (strcmp_i(XPARSE_REGEXP_SYMBOLS[begin_of_match], sp, 1)) {
        codegen_begin(parser);
        sp ++;
    }

    return sp - regexp;
}
