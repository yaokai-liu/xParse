//
// Project Name: xParse
// Filename: load_imm.c
// Creator: Yaokai Liu
// Create Date: 2023-10-24
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "pseudo.h"

#define MAX_UNSIGNED_SHORT ((1 << 16) - 1)

#define LOAD_IMM_INST(_i) do { \
    instruction.load_imm.opcode = inst_load_imm; \
    instruction.load_imm.rd = reg; \
    instruction.load_imm.imm = count_s[_i]; \
    MemSpace.push(inst_space, &instruction); \
} while(false)

#define SHIFT_MOVE_INST(_c) do { \
    instruction.msl_reg.opcode = inst_shift_move; \
    instruction.msl_reg.rd = reg; \
    instruction.msl_reg.rs = reg; \
    instruction.msl_reg.offset = (_c); \
    MemSpace.push(inst_space, &instruction); \
} while(false)

inline xInt pseudo_load_imm64(xuLong value, xuByte reg, mem_space *inst_space) {
    if (!vm_writable(reg)) { return -1; }
    inst instruction; xuShort count_s[4];
    count_s[0] = ((value) >> (00)) & MAX_UNSIGNED_SHORT;
    count_s[1] = ((value) >> (16)) & MAX_UNSIGNED_SHORT;
    count_s[2] = ((value) >> (32)) & MAX_UNSIGNED_SHORT;
    count_s[3] = ((value) >> (48)) & MAX_UNSIGNED_SHORT;

    xSize length = MemSpace.len(inst_space);
    LOAD_IMM_INST(3);
    SHIFT_MOVE_INST(8);
    LOAD_IMM_INST(2);
    SHIFT_MOVE_INST(8);
    LOAD_IMM_INST(1);
    SHIFT_MOVE_INST(8);
    LOAD_IMM_INST(0);
    return (xInt) (MemSpace.len(inst_space) - length);
}


inline xInt pseudo_load_imm32(xuInt value, xuByte reg, mem_space *inst_space) {
    if (!vm_writable(reg)) { return -1; }
    inst instruction; xuShort count_s[4];
    count_s[0] = ((value) >> (00)) & MAX_UNSIGNED_SHORT;
    count_s[1] = ((value) >> (16)) & MAX_UNSIGNED_SHORT;

    xSize length = MemSpace.len(inst_space);
    SHIFT_MOVE_INST(-8);
    LOAD_IMM_INST(1);
    SHIFT_MOVE_INST(8);
    LOAD_IMM_INST(0);
    return (xInt) (MemSpace.len(inst_space) - length);
}
