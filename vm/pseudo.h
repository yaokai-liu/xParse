//
// Project Name: xParse
// Filename: pseudo.h
// Creator: Yaokai Liu
// Create Date: 2023-10-24
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#ifndef XPARSE_PSEUDO_H
#define XPARSE_PSEUDO_H

#include "xtypes.h"
#include "mem_space.h"
#include "inst.h"
#include "vm.h"

static xBool BUSY_REGISTERS[15] = {};

xVoid pseudo_load_imm64(xuLong value, xuByte reg, mem_space *inst_space);
xVoid pseudo_load_imm32(xuInt value, xuByte reg, mem_space *inst_space);
xVoid pseudo_push(xuByte reg, mem_space *inst_space);
xVoid pseudo_pop(xuByte reg, mem_space *inst_space);
xVoid pseudo_clear(xuByte reg, mem_space *inst_space);

xVoid pseudo_jump(xuByte opcode, xuLong base, xInt offset, mem_space *inst_space);

#endif //XPARSE_PSEUDO_H
