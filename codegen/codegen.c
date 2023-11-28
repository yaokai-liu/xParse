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

xVoid codegen_sequence(xParser *parser, char_t *start, xSize n_plains) {
    inst instruction;
    pseudo_load_imm64(n_plains, vm_count_reg, parser->INST_SPACE[parser->used]);
    char_t * locate = MemSpace.real2virt(parser->CHAR_SPACE, start);
    pseudo_load_imm64((xuLong) locate, vm_reg_alloc, parser->INST_SPACE[parser->used]);
    instruction.match_reg = (struct inst_match_reg){
            .opcode = inst_seq_reg,
            .reg = vm_reg_alloc,
            .offset = 0,
    };
    MemSpace.push(parser->INST_SPACE[parser->used], &instruction);
}


xVoid codegen_inverse(xParser * parser) {
    inst instruction;
    instruction.set_value = (struct inst_set_value) {
        .opcode = inst_set_ma_mode,
        .value = VM_SET_STATUS_MATCH_MODE_INVERSE,
    };
    MemSpace.push(parser->INST_SPACE[parser->used], &instruction);
}


xVoid codegen_begin(xParser * parser) {
    inst instruction;
    instruction.arith = (struct inst_arith) {
            .opcode = inst_cmp,
            .rs1 = vm_inst_reg,
            .rs2 = vm_src_reg,
            .rd = vm_zero_reg // TODO:
    };
    MemSpace.push(parser->INST_SPACE[parser->used], &instruction);
    instruction.jump = (struct inst_jump) {
        .opcode = inst_jump_if_ne,
        .offset = vm_zero_reg, // TODO:
    };
    MemSpace.push(parser->INST_SPACE[parser->used], &instruction);
}
