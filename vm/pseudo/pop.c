//
// Project Name: xParse
// Filename: pop.c
// Creator: Yaokai Liu
// Create Date: 2023-10-24
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "pseudo.h"

inline xInt pseudo_pop(xuByte reg, mem_space *inst_space) {
    inst instruction;

    instruction.msl_reg.opcode = inst_load;
    instruction.msl_reg.rd = reg;
    instruction.msl_reg.rs = vm_stack_reg;
    instruction.msl_reg.offset = 0;
    MemSpace.push(inst_space, &instruction);

    instruction.arith_imm.opcode = inst_add_u_i;
    instruction.arith_imm.rd = vm_stack_reg;
    instruction.arith_imm.imm = sizeof(xuLong);
    MemSpace.push(inst_space, &instruction);

    return 2;
}