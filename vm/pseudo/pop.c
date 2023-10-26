//
// Project Name: xParse
// Filename: pop.c
// Creator: Yaokai Liu
// Create Date: 2023-10-24
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "pseudo.h"

inline xVoid pseudo_pop(xuByte reg, mem_space *inst_space) {
    inst instruction;
    instruction.los_reg = (struct inst_los_reg) {
            .opcode = inst_load,
            .rd = reg,
            .rs = vm_stack_reg,
            .offset = 0,
    };
    MemSpace.push(inst_space, &instruction);
    instruction.arith_imm = (struct inst_arith_imm) {
            .opcode = inst_add_i,
            .rd = vm_stack_reg,
            .imm = sizeof(xuLong),
    };
    MemSpace.push(inst_space, &instruction);
}