//
// Project Name: xParse
// Filename: plains.c
// Creator: Yaokai Liu
// Create Date: 2023-10-23
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "misc.h"
#include "string.h"
#include "escape.h"

#define MAX_SHORT ((1 << 15) - 1)

xSize parse_plains(xParser * parser, char_t *regexp) {
    char_t *sp = regexp;
    trace_enter();
#define CHECK_NON_PLAIN stridx(NON_PLAIN, *sp)
    while (*sp && CHECK_NON_PLAIN < 0) sp++;
#undef CHECK_NON_PLAIN
    if (sp > regexp) {
        MmeSpace.concat(parser->CHAR_SPACE, regexp, sp - regexp);
    }
    trace_leave();
    return sp - regexp;
}
