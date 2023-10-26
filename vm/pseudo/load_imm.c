//
// Project Name: xParse
// Filename: load_imm.c
// Creator: Yaokai Liu
// Create Date: 2023-10-24
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "pseudo.h"

#define MAX_SHORT ((1 << 16) - 1)

inline xVoid pseudo_load_imm64(xuLong value, xuByte reg, mem_space *inst_space) {
    inst instruction;
    xuInt reg_idx = 0;
    xuByte temp_reg;
    if (reg >= vm_arith_reg_start && reg <= vm_arith_reg_end) {
        BUSY_REGISTERS[reg - vm_arith_reg_start] |= 1;
    }
    while (reg_idx < 15) {
        if (!BUSY_REGISTERS[reg_idx]) break;
        reg_idx ++;
    }
    if (reg_idx < 15) {
        temp_reg = vm_arith_reg_start + reg_idx;
    } else if (vm_arith_reg_1 == reg) {
        pseudo_push(vm_arith_reg_2, inst_space);
        temp_reg = vm_arith_reg_2;
    } else {
        pseudo_push(vm_arith_reg_1, inst_space);
        temp_reg = vm_arith_reg_1;
    }
    pseudo_clear(temp_reg, inst_space);

    xuShort count_s[4];
    count_s[0] = ((value) >> (00)) & MAX_SHORT;
    count_s[1] = ((value) >> (16)) & MAX_SHORT;
    count_s[2] = ((value) >> (32)) & MAX_SHORT;
    count_s[3] = ((value) >> (48)) & MAX_SHORT;

    for (xuInt i = 0; i < 4; i ++) {
        if (count_s[i] == 0) {
            continue;
        }
        instruction.load_imm = (struct inst_load_imm) {
                .opcode = inst_load_imm,
                .rd = temp_reg,
                .imm = count_s[i],
        };
        MemSpace.push(inst_space, &instruction);
        instruction.los_reg = (struct inst_los_reg) {
            .opcode = inst_sh_mv,
            .rd = reg,
            .rs = temp_reg,
            .offset =(xByte) (16 * i),
        };
        MemSpace.push(inst_space, &instruction);
    }
    if (reg_idx < 15) {
        BUSY_REGISTERS[reg_idx] = 0;
    } else {
        pseudo_pop(temp_reg, inst_space);
    }
}


inline xVoid pseudo_load_imm32(xuInt value, xuByte reg, mem_space *inst_space) {
    inst instruction;
    xuInt reg_idx = 0;
    xuByte temp_reg;
    if (reg >= vm_arith_reg_start && reg <= vm_arith_reg_end) {
        BUSY_REGISTERS[reg - vm_arith_reg_start] |= 1;
    }
    while (reg_idx < 15) {
        if (!BUSY_REGISTERS[reg_idx]) break;
        reg_idx ++;
    }
    if (reg_idx < 15) {
        temp_reg = vm_arith_reg_start + reg_idx;
    } else if (vm_arith_reg_1 == reg) {
        pseudo_push(vm_arith_reg_2, inst_space);
        temp_reg = vm_arith_reg_2;
    } else {
        pseudo_push(vm_arith_reg_1, inst_space);
        temp_reg = vm_arith_reg_1;
    }
    pseudo_clear(temp_reg, inst_space);

    xuShort count_s[2];
    count_s[0] = ((value) >> (00)) & MAX_SHORT;
    count_s[1] = ((value) >> (16)) & MAX_SHORT;

    instruction.load_imm = (struct inst_load_imm) {
            .opcode = inst_load_imm,
            .rd = reg,
            .imm = count_s[0],
    };
    MemSpace.push(inst_space, &instruction);

    if (count_s[1] != 0) {
        instruction.load_imm = (struct inst_load_imm) {
                .opcode = inst_load_imm,
                .rd = temp_reg,
                .imm = count_s[0],
        };
        MemSpace.push(inst_space, &instruction);
        instruction.los_reg = (struct inst_los_reg) {
                .opcode = inst_sh_mv,
                .rd = reg,
                .rs = temp_reg,
                .offset =(xByte) 16,
        };
        MemSpace.push(inst_space, &instruction);
    }

    if (reg_idx < 15) {
        BUSY_REGISTERS[reg_idx] = 0;
    } else {
        pseudo_pop(temp_reg, inst_space);
    }
}
