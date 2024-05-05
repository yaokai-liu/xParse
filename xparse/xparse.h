//
// Project Name: xParse
// Filename: xparse.h
// Creator: Yaokai Liu
// Create Date: 2023-10-23
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#ifndef XPARSE_XPARSE_H
#define XPARSE_XPARSE_H

#include "mem_space.h"
#include "structs.h"

typedef struct {
    const struct Allocator *allocator;
    mem_space * INST_SPACE[2]; // mem_space<inst>
    mem_space * CHAR_SPACE; // mem_space<char_t>
    mem_space * GROUP_SPACE; // mem_space<Refer *>
    mem_space * LABEL_SPACE; // mem_space<Refer *>
    mem_space * CAPTURE_SPACE; // mem_space<Capture>

    xuInt reg_alloc_count;  // assert(reg_alloc_count > 32)
    xuInt current_level;
    const char_t * err_pos;
    xSize err_len;
    xuInt used:1;
    xuInt unused:1;
} xParser;

xInt xParser_init(xParser * parser, const struct Allocator * allocator);
xInt xParser_release(xParser * parser);

#ifdef XPARSE_DEBUG

#include <stdio.h>
#ifndef XPARSE_TRACE_OUT
#define XPARSE_TRACE_OUT stdout
#endif
#define trace_enter() do { \
    fprintf(XPARSE_TRACE_OUT, "%sEnter <%s>, sp = %ld;\n", \
    &trace_indent[0], __FUNCTION__, sp - cur_text); \
    trace_indent.push(xReChar('-')); \
} while(false)

#define trace_leave() do { \
    trace_indent.pop(nullptr); \
    fprintf(XPARSE_TRACE_OUT, "%sLeave <%s>, sp = %ld;\n", \
    &trace_indent[0], __FUNCTION__, sp - cur_text); \
} while(0)
#else
#define trace_enter() do {0;} while(0)
#define trace_leave() do {0;} while(0)
#endif

#define check(expr, expect_len) do { \
    if (!(expr)) { \
        parser->err_pos = sp; \
        parser->err_len = expect_len; \
        return 0; \
    } \
} while (false)

#endif //XPARSE_XPARSE_H
