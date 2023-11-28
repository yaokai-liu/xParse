//
// Project Name: xParse
// Filename: single_escape.c
// Creator: Yaokai Liu
// Create Date: 2023-10-23
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "misc.h"
#include "string.h"
#include "symbols.h"
#include "escape.h"

xSize parse_single_escape(xParser * parser, const char_t * const regexp) {
    const char_t *sp = regexp;
    trace_enter();

    regexp_sym_eat(escape);

    xInt index = stridx_o(SINGLE_ESCAPE, *sp);
    if (index >= 0) {
        // Since NON_PLAIN is corresponded with SINGLE_ESCAPE,
        // it will use NON_PLAIN to save space of jump table here.
        MemSpace.push(parser->CHAR_SPACE, (xVoid *)&(NON_PLAIN[index]));
        sp++;
        trace_leave();
        return sp - regexp;
    }
    switch (*sp) {
        case 'u': {
            // TODO: parse unicode
        }
        case 'x': {
            // TODO: parse unsigned hexadecimal integer <max 64 bit>
        }
        default: {
            check(0 == 1, 1);
        }
    }
}
