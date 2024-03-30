//
// Project Name: xParse
// Filename: clear.c
// Creator: Yaokai Liu
// Create Date: 2023-10-24
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "pseudo.h"

inline xInt pseudo_clear(xuByte reg, mem_space *inst_space) {
    if (!vm_writable(reg)) { return -1; }
    inst instruction;
    instruction.msl_reg.opcode = inst_shift_move;
    instruction.msl_reg.rd = reg;
    instruction.msl_reg.rs = vm_zero_reg;
    instruction.msl_reg.offset = 0;
    MemSpace.push(inst_space, &instruction);
    return 1;
}
