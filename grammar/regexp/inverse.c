//
// Project Name: xParse
// Filename: inverse.c
// Creator: Yaokai Liu
// Create Date: 2023-10-23
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//
#include "regexp.h"
#include "symbols.h"
#include "string.h"
#include "codegen.h"

xSize compile_inverse(xParser * parser, const char_t * const regexp) {
    const char_t *sp = regexp;

    if (strcmp_i(XPARSE_REGEXP_SYMBOLS[inverse_match], sp, 1)) {
        codegen_inverse(parser);
        sp ++;
    }

    return sp - regexp;
}
