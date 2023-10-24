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
    instruction.los_reg = (struct inst_los_reg) {
        .opcode = inst_sh_mv,
        .rd = reg,
        .rs = vm_zero_reg,
        .offset = 0,
    };
    MmeSpace.push(inst_space, &instruction);
}