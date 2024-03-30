//
// Project Name: xParse
// Filename: push.c
// Creator: Yaokai Liu
// Create Date: 2023-10-24
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "pseudo.h"

inline xInt pseudo_push(xuByte reg, mem_space *inst_space) {
    inst instruction;

    instruction.arith_imm.opcode = inst_sub_u_i;
    instruction.arith_imm.rd = vm_stack_reg;
    instruction.arith_imm.imm = sizeof(xuLong);
    MemSpace.push(inst_space, &instruction);

    instruction.msl_reg.opcode = inst_store;
    instruction.msl_reg.rd = vm_stack_reg;
    instruction.msl_reg.rs = reg;
    instruction.msl_reg.offset = sizeof(xuLong);
    MemSpace.push(inst_space, &instruction);

    return 2;
}