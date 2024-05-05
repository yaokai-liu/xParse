/**
 * Project Name: Project
 * Module Name: xparse
 * Filename: xparse.c
 * Creator: Yaokai Liu
 * Create Date: 2024-04-04
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "xparse.h"
#include "xtypes.h"
#include "inst.h"

xInt xParser_init(xParser * parser, const struct Allocator * const allocator) {
    parser->allocator = allocator;
    parser->INST_SPACE[0] = MemSpace.new("INST 1", allocator, sizeof(inst));
    parser->INST_SPACE[1] = MemSpace.new("INST 2", allocator, sizeof(inst));
    parser->CHAR_SPACE = MemSpace.new("CHAR", allocator, sizeof(char_t));
    parser->CAPTURE_SPACE = MemSpace.new("CAPTURE", allocator, sizeof(Capture));
    parser->GROUP_SPACE = MemSpace.new("GROUP", allocator, sizeof(Refer *));
    parser->LABEL_SPACE = MemSpace.new("LABEL", allocator, sizeof(Refer *));

    parser->reg_alloc_count = 33;
    parser->current_level = 0;
    parser->err_pos = nullptr;
    parser->err_len = 0;
}

xInt xParser_release(xParser * parser) {
    MemSpace.del(parser->INST_SPACE[0]);
    MemSpace.del(parser->INST_SPACE[1]);
    MemSpace.del(parser->CHAR_SPACE);
    MemSpace.del(parser->CAPTURE_SPACE);
    MemSpace.del(parser->GROUP_SPACE);
    MemSpace.del(parser->LABEL_SPACE);
}
