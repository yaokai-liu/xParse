//
// Project Name: xParse
// Filename: jump.c
// Creator: Yaokai Liu
// Create Date: 2023-11-06
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "pseudo.h"

xVoid pseudo_jump(xuByte opcode, xuLong base, xInt offset, mem_space *inst_space) {
    inst instruction;
    if (base) {
        pseudo_load_imm64(base, vm_jump_base_reg, inst_space);
    }
    instruction.jump = (struct inst_jump) {
            .opcode = opcode,
            .offset = offset,
    };
    MemSpace.push(inst_space, &instruction);
}
