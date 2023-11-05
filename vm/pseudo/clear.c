//
// Project Name: xParse
// Filename: clear.c
// Creator: Yaokai Liu
// Create Date: 2023-10-24
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "pseudo.h"

inline xVoid pseudo_clear(xuByte reg, mem_space *inst_space) {
    inst instruction;
    instruction.msl_reg = (struct inst_msl_reg) {
        .opcode = inst_shift_move,
        .rd = reg,
        .rs = vm_zero_reg,
        .offset = 0,
    };
    MemSpace.push(inst_space, &instruction);
}
