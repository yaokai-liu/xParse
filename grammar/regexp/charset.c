//
// Project Name: xParse
// Filename: charset.c
// Creator: Yaokai Liu
// Create Date: 2023-10-24
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//
#include "regexp.h"
#include "symbols.h"
#include "misc.h"


xSize compile_charset(xParser * parser, const char_t * const regexp) {
    const char_t *sp = regexp;
    trace_enter();

    check(*sp == begin_charset, 1);

    sp += parse_charset(parser, sp, parser->INST_SPACE[parser->used]);
    if (parser->err_len) { return 0; }

    trace_leave();
    return sp - regexp;
}
