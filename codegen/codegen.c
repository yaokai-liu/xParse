//
// Project Name: xParse
// Filename: codegen.c
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "codegen.h"
#include "inst.h"
#include "vm.h"
#include "pseudo.h"
#include "xdef.h"

#define USED_INST_ARRAY (parser->INST_SPACE[parser->used])
#define ALLOC_REG       (parser->reg_alloc_count)

xInt codegen_sequence(xParser *parser, char_t *start, xSize n_plains) {
    inst instruction;
    switch (n_plains) {
        case 0: { return -1; }
        case 1: {
            instruction.match_lit.opcode = inst_seq_lit1,
            instruction.match_lit.target[0] = start[0];
            instruction.match_lit.target[1] = '\0';
            instruction.match_lit.target[2] = '\0';
            return 1;
        }
        case 2: {
            instruction.match_lit.opcode = inst_seq_lit2,
            instruction.match_lit.target[0] = start[0];
            instruction.match_lit.target[1] = start[1];
            instruction.match_lit.target[2] = '\0';
            return 1;
        }
        case 3: {
            instruction.match_lit.opcode = inst_seq_lit3,
            instruction.match_lit.target[0] = start[0];
            instruction.match_lit.target[1] = start[1];
            instruction.match_lit.target[2] = start[2];
            return 1;
        }
        default: {
            xInt inst_count = 0;
            inst_count += pseudo_load_imm64(n_plains, vm_count_reg, USED_INST_ARRAY);
            char_t * locate = MemSpace.real2virt(parser->CHAR_SPACE, start);
            inst_count += pseudo_load_imm64((xuLong) locate, ALLOC_REG, USED_INST_ARRAY);
            ALLOC_REG ++;

            instruction.match_reg.opcode = inst_seq_reg;
            instruction.match_reg.reg = ALLOC_REG;
            instruction.match_reg.offset = 0;
            MemSpace.push(USED_INST_ARRAY, &instruction);
            inst_count ++;
            ALLOC_REG ++;
            return inst_count;
        }
    }
}


xInt codegen_inverse(xParser * parser) {
    inst instruction;

    instruction.set_value.opcode = inst_set_ma_mode;
    instruction.set_value.value = MATCH_INVERSE_MODE;
    MemSpace.push(USED_INST_ARRAY, &instruction);

    return 1;
}


xInt codegen_begin(xParser * parser) {
    inst instruction;

    instruction.arith_reg.opcode = inst_cmp_u;
    instruction.arith_reg.rs1 = vm_inst_reg;
    instruction.arith_reg.rs2 = vm_src_reg;
    instruction.arith_reg.rd = ALLOC_REG;
    MemSpace.push(USED_INST_ARRAY, &instruction);

    instruction.jump.opcode = inst_jump_if_ne;
    instruction.jump.offset = vm_zero_reg;
    MemSpace.push(USED_INST_ARRAY, &instruction);

    return 1;
}
