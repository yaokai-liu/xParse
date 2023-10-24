//
// Project Name: xParse
// Filename: sequence.c
// Creator: Yaokai Liu
// Create Date: 2023-10-23
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "regexp.h"
#include "symbols.h"
#include "misc.h"
#include "string.h"
#include "codegen.h"

xSize compile_sequence(xParser * parser, char_t * regexp) {
    char_t *sp = regexp;
    trace_enter();

    xSize start = MmeSpace.size(parser->CHAR_SPACE);

    __parse_plains:
    sp += parse_plains(parser, sp);
    if (parser->err_len > 0) return 0;
    if (strcmp_i(sp, XPARSE_REGEXP_SYMBOLS[escape], 1)) {
        xSize offset = parse_single_escape(parser, sp);
        if (offset > 0) goto __parse_plains;
    }

    check(sp > regexp, 1);

    xSize n_plains = MmeSpace.size(parser->CHAR_SPACE) - start;
    codegen_sequence(parser, MmeSpace.real_addr(parser->CHAR_SPACE, start), n_plains);

    trace_leave();
    return sp - regexp;
}

