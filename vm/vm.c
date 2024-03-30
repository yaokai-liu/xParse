//
// Project Name: xParse
// Filename: vm.c
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "vm.h"
#include "inst.h"
#include "mem_manager.h"
#include "string.h"
#include "xdef.h"

#define __XVM xVirtualMachine // NOLINT(*-reserved-identifier)
__XVM * vm_new(const struct Allocator* allocator);
xVoid vm_init(__XVM * vm, const struct Allocator* allocator);
xVoid vm_execute(__XVM * vm, inst * instruction);

const struct __XPARSE_VM_Method__ VM = {
        .new = vm_new,
        .init = vm_init,
        .execute = vm_execute,
};

__XVM * vm_new(const struct Allocator* allocator) {
    __XVM * vm =(__XVM *) allocator->malloc(sizeof(__XVM));
    vm_init(vm, allocator);
    return vm;
}

#define vm_reg(_name) (vm->registers._##_name##_reg)

xVoid vm_init(__XVM * vm, const struct Allocator* allocator) {
    vm->allocator = allocator;
    // memories init
    vm->manager = MemManager.new("Virtual Machine", allocator);
#define INST_MEM_ID         0
    MemManager.new_space(vm->manager, "INSTRUCTION", sizeof(inst));
#define EA_MEM_ID          1
    MemManager.new_space(vm->manager, "REGEX RESET STACK", sizeof(char_t *));
#define RA_MEM_ID           2
    MemManager.new_space(vm->manager, "RETURN ADDRESS STACK", sizeof(xVoid *));
#define CO_MEM_ID           3
    MemManager.new_space(vm->manager, "CALL OFFSET STACK", sizeof(xuLong));
#define RAM_MEM_ID          4
    MemManager.new_space(vm->manager, "RAM", sizeof(xuLong));
#define CAP_MEM_ID          5
    MemManager.new_space(vm->manager, "CAPTURE", sizeof(xuLong));

    // register init
    vm_reg(regex_level) = 0;
    vm_reg(call_level) = 0;
    vm_reg(inst) = 0;
    vm_reg(ret_addr) = 0;
    vm_reg(src) = 0;
    vm_reg(status).value = 0;
    vm_reg(count) = 0;
}
#define vm_virt2real(_ptr) MemManager.virt2real(vm->manager, _ptr)
#define vm_real2virt(_ptr) MemManager.real2virt(vm->manager, _ptr)
#define vm_raise(_error_type) do { \
vm_reg(status).fields.TRAP_FLAG = (_error_type); \
} while (false)
#define vm_assert(_the_bool) do { \
if (!(_the_bool)) {vm_raise(TRAP_ILLEGAL_INST_ERROR); return; } \
} while (false)
#define vm_get_reg(reg_id) ( \
((reg_id) < VM_ARITH_REG_END) ? ((xuLong *)&vm->registers + (reg_id)) : nullptr \
)

typedef xVoid (*executor)(__XVM * vm, inst * inst);
const static executor VM_EXECUTORS[256];

xVoid vm_execute(__XVM * vm, inst * instruction) {
    VM_EXECUTORS[instruction->single.opcode](vm, instruction);
}

xVoid vm_run(__XVM * vm) {

}

xVoid vm_execute_nop(__XVM * vm, inst * inst) {
}

xVoid vm_execute_success(__XVM * vm, inst * inst) {
    //TODO: vm execute success
}

xVoid vm_execute_failed(__XVM * vm, inst * inst) {
    //TODO: vm execute failed
}

#define VM_REGEXP_MODE ()
#define VM_PROGRAM_MODE (0b0000'0000)

xVoid vm_execute_set_vm_mode(__XVM * vm, inst * inst) {
    vm_assert(inst->set_value.value < 2);
    vm_reg(status).fields.EXECUTE_MODE = inst->set_value.value;
}

xVoid vm_execute_set_ma_mode(__XVM * vm, inst * inst) {
    vm_assert(inst->set_value.value < 2);
    vm_reg(status).fields.MATCH_MODE = inst->set_value.value;

}

xVoid vm_execute_clear_ma_flag(__XVM * vm, inst * inst) {
    vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_UNSET;
}

xVoid vm_execute_clear_cmp_flag(__XVM * vm, inst * inst) {
    vm_reg(status).fields.COMPARE_FLAG = COMPARE_FLAG_NC;
}

xVoid vm_execute_seq_lit(__XVM * vm, inst * inst) {
    if (vm_reg(src)[0] == inst->match_lit.target[0]) {
        vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_MATCHED;
    } else {
        vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_NOT_MATCHED;
    }
}

xVoid vm_execute_seq_lit2(__XVM * vm, inst * inst) {
    if (vm_reg(src)[0] == inst->match_lit.target[0]
        && vm_reg(src)[1] == inst->match_lit.target[1]) {
        vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_MATCHED;
    } else {
        vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_NOT_MATCHED;
    }
}

xVoid vm_execute_seq_lit3(__XVM * vm, inst * inst) {
    if (vm_reg(src)[0] == inst->match_lit.target[0]
        && vm_reg(src)[1] == inst->match_lit.target[1]
        && vm_reg(src)[2] == inst->match_lit.target[2]) {
        vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_MATCHED;
    } else {
        vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_NOT_MATCHED;
    }
}

xVoid vm_execute_set_lit(__XVM * vm, inst * inst) {
    if (vm_reg(src)[0] == inst->match_lit.target[0]) {
        vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_MATCHED;
    } else {
        vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_NOT_MATCHED;
    }
}

xVoid vm_execute_set_lit2(__XVM * vm, inst * inst) {
    if (vm_reg(src)[0] == inst->match_lit.target[0]
        || vm_reg(src)[0] == inst->match_lit.target[1]) {
        vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_MATCHED;
    } else {
        vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_NOT_MATCHED;
    }
}

xVoid vm_execute_set_lit3(__XVM * vm, inst * inst) {
    if (vm_reg(src)[0] == inst->match_lit.target[0]
        || vm_reg(src)[0] == inst->match_lit.target[1]
        || vm_reg(src)[0] == inst->match_lit.target[2]) {
        vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_MATCHED;
    } else {
        vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_NOT_MATCHED;
    }
}

xVoid vm_execute_range_lit(__XVM * vm, inst * inst) {
    if (*vm_reg(src) >= inst->match_lit.target[0]
      && *vm_reg(src) <= inst->match_lit.target[1]) {
        vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_MATCHED;
    } else {
        vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_NOT_MATCHED;
    }
}

xVoid vm_execute_seq_reg(__XVM * vm, inst * inst) {
    char_t ** target = (char_t **) vm_get_reg(inst->match_reg.reg);
    vm_assert(target != nullptr);
    xuInt count = inst->match_reg.offset ? inst->match_reg.offset : vm_reg(count);
    xBool _b = strcmp_i(vm_reg(src), *target, count);
    vm_reg(status).fields.MATCH_FLAG = _b ? MATCH_FLAG_MATCHED : MATCH_FLAG_NOT_MATCHED;
}

xVoid vm_execute_set_reg(__XVM * vm, inst * inst) {
    char_t ** target = (char_t **) vm_get_reg(inst->match_reg.reg);
    vm_assert(target != nullptr);
    xuInt count = inst->match_reg.offset ? inst->match_reg.offset : vm_reg(count);
    xInt _b = stridx_i(*target, *vm_reg(src), count);
    vm_reg(status).fields.MATCH_FLAG = _b ? MATCH_FLAG_MATCHED : MATCH_FLAG_NOT_MATCHED;
}

xVoid vm_execute_range_reg(__XVM * vm, inst * inst) {
    struct range { char_t start; char_t end; };
    struct range ** target = (struct range **) vm_get_reg(inst->match_reg.reg);
    vm_assert(target != nullptr);
    xuInt count = inst->match_reg.offset ? inst->match_reg.offset : vm_reg(count);
    for (; vm_reg(count) > 0; count --) {
        if (target[vm_reg(count) - 1]->start <= *vm_reg(src)
           && target[vm_reg(count) - 1]->end >= *vm_reg(src)) {
            vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_MATCHED;
            return;
        }
    }
    vm_reg(status).fields.MATCH_FLAG = MATCH_FLAG_NOT_MATCHED;
}

xVoid vm_execute_enter(__XVM * vm, inst * inst) {
    mem_space * space = MemManager.get_space(vm->manager, EA_MEM_ID);
    MemSpace.push(space, &(vm_reg(reset_src)));
    vm_reg(reset_src) = vm_reg(src);
    vm_reg(regex_level) ++;
}

xVoid vm_execute_reset(__XVM * vm, inst * inst) {
    vm_reg(src) = vm_reg(reset_src);
}

xVoid vm_execute_exit(__XVM * vm, inst * inst) {
    if (vm_reg(regex_level) == 0) { vm_raise(TRAP_RESET_LEVEL_ERROR); return ; }
    mem_space * space = MemManager.get_space(vm->manager, EA_MEM_ID);
    MemSpace.pop(space, &(vm_reg(reset_src)));
    vm_reg(regex_level) --;
}

#define jump_to(_offset)   do { vm_reg(inst) = vm_reg(jump_base) + _offset; } while (false)

xVoid vm_execute_jump_directly(__XVM * vm, inst * inst) {
    jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_eq(__XVM * vm, inst * inst) {
    if (vm_reg(status).fields.COMPARE_FLAG == COMPARE_FLAG_EQ)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_ne(__XVM * vm, inst * inst) {
    if (vm_reg(status).fields.COMPARE_FLAG == COMPARE_FLAG_NE)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_gt(__XVM * vm, inst * inst) {
    if (vm_reg(status).fields.COMPARE_FLAG == COMPARE_FLAG_GT)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_lt(__XVM * vm, inst * inst) {
    if (vm_reg(status).fields.COMPARE_FLAG == COMPARE_FLAG_LT)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_ge(__XVM * vm, inst * inst) {
    if (vm_reg(status).fields.COMPARE_FLAG == COMPARE_FLAG_EQ
        || vm_reg(status).fields.COMPARE_FLAG == COMPARE_FLAG_GT)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_le(__XVM * vm, inst * inst) {
    if (vm_reg(status).fields.COMPARE_FLAG == COMPARE_FLAG_EQ
        || vm_reg(status).fields.COMPARE_FLAG == COMPARE_FLAG_LT)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_ma(__XVM * vm, inst * inst) {
    if (vm_reg(status).fields.MATCH_FLAG == MATCH_FLAG_MATCHED)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_nm(__XVM * vm, inst * inst) {
    if (vm_reg(status).fields.MATCH_FLAG == MATCH_FLAG_NOT_MATCHED)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_call(__XVM * vm, inst * inst) {
    mem_space * space;

    space = MemManager.get_space(vm->manager, RA_MEM_ID);
    MemSpace.push(space, &vm_reg(ret_addr));
    vm_reg(ret_addr) = vm_reg(inst) + 1;

    space = MemManager.get_space(vm->manager, CO_MEM_ID);
    MemSpace.push(space, &vm_reg(call_offset));
    vm_reg(call_offset) = (xuInt) inst->jump.offset;

    vm_reg(inst) = vm_reg(call_vec)[inst->jump.offset];
    vm_reg(call_level) ++;
}

xVoid vm_execute_ret(__XVM * vm, inst * inst) {
    if (inst->jump.offset == vm_reg(call_offset)) {
        mem_space * space;

        space = MemManager.get_space(vm->manager, RA_MEM_ID);
        MemSpace.pop(space, &(vm_reg(ret_addr)));

        space = MemManager.get_space(vm->manager, CO_MEM_ID);
        MemSpace.pop(space, &(vm_reg(call_offset)));

        vm_reg(inst) = vm_reg(ret_addr);
        vm_reg(call_level) --;
    }
}

#define write_reg(_rd, _value) do {*(_rd) = (_rd) ? (_value) : 0;} while (false)

xVoid vm_execute_load(__XVM * vm, inst * inst) {
    vm_assert(vm_writable(inst->msl_reg.rd));
    xuLong * rd = vm_get_reg(inst->msl_reg.rd);
    xuLong * rs = vm_get_reg(inst->msl_reg.rs);
    rs = vm_virt2real(* (xVoid **) rs);
    write_reg(rd, ((xuLong *)rs)[inst->msl_reg.offset]);
}

xVoid vm_execute_store(__XVM * vm, inst * inst) {
    xuLong * rd = vm_get_reg(inst->msl_reg.rd);
    xuLong * rs = vm_get_reg(inst->msl_reg.rs);
    rd = vm_virt2real(* (xVoid **) rd);
    vm_assert(rd != nullptr);
    ((xuLong *)rd)[inst->msl_reg.offset] = * rs;
}

xVoid vm_execute_shift_move(__XVM * vm, inst * inst) {
    vm_assert(vm_writable(inst->msl_reg.rd));
    xuLong * rd = vm_get_reg(inst->msl_reg.rd);
    xuLong * rs = vm_get_reg(inst->msl_reg.rs);
    vm_assert(rd != nullptr && rs != nullptr);
    write_reg(rd, (*rs) << (inst->msl_reg.offset));
}

xVoid vm_execute_load_imm(__XVM * vm, inst * inst) {
    vm_assert(vm_arithmetic(inst->load_imm.rd));
    xuLong * rd = vm_get_reg(inst->load_imm.rd);
    vm_assert(rd != nullptr);
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    ((xuShort *) rd)[0] = inst->load_imm.imm;
#elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    ((xuShort *) rd)[3] = inst->load_imm.imm;
#endif
}

#define vm_execute_arith_u(inst_name, expr, the_assert) \
xVoid vm_execute_##inst_name(__XVM * vm, inst * inst) { \
    vm_assert(vm_arithmetic(inst->arith_reg.rd)); \
    xuLong * rd  = vm_get_reg(inst->arith_reg.rd); \
    xuLong * rs1 = vm_get_reg(inst->arith_reg.rs1); \
    xuLong * rs2 = vm_get_reg(inst->arith_reg.rs2); \
    vm_assert(rd != nullptr && rs1 != nullptr && rs2 != nullptr); \
    vm_assert(the_assert); \
    write_reg(rd, expr); \
}

#define vm_execute_arith_s(inst_name, expr, the_assert) \
xVoid vm_execute_##inst_name(__XVM * vm, inst * inst) { \
    vm_assert(vm_arithmetic(inst->arith_reg.rd)); \
    xLong * rd  = (xLong *) vm_get_reg(inst->arith_reg.rd); \
    xLong * rs1 = (xLong *) vm_get_reg(inst->arith_reg.rs1); \
    xLong * rs2 = (xLong *) vm_get_reg(inst->arith_reg.rs2); \
    vm_assert(rd != nullptr && rs1 != nullptr && rs2 != nullptr); \
    vm_assert(the_assert); \
    write_reg(rd, expr); \
}

vm_execute_arith_u(b_and, (*rs1) & (*rs2), true)
vm_execute_arith_u(b_or, (*rs1) | (*rs2), true)
vm_execute_arith_u(b_xor, (*rs1) ^ (*rs2), true)
vm_execute_arith_u(b_lsh, (*rs1) << (*rs2), true)
vm_execute_arith_u(b_rsh, (*rs1) >> (*rs2), true)
vm_execute_arith_u(b_inv, !(*rs1), true)

vm_execute_arith_s(add_s, (*rs1) + (*rs2), true)
vm_execute_arith_s(sub_s, (*rs1) - (*rs2), true)
vm_execute_arith_s(mul_s, (*rs1) * (*rs2), true)
vm_execute_arith_s(div_s, (*rs1) / (*rs2), (*rs2) != 0)
vm_execute_arith_s(mod_s, (*rs1) % (*rs2), (*rs2) != 0)
vm_execute_arith_u(add_u, (*rs1) + (*rs2), true)
vm_execute_arith_u(sub_u, (*rs1) - (*rs2), true)
vm_execute_arith_u(mul_u, (*rs1) * (*rs2), true)
vm_execute_arith_u(div_u, (*rs1) / (*rs2), (*rs2) != 0)
vm_execute_arith_u(mod_u, (*rs1) % (*rs2), (*rs2) != 0)

#define vm_execute_arith_imm_s(inst_name, expr, the_assert) \
xVoid vm_execute_##inst_name##_imm(__XVM * vm, inst * inst) { \
    vm_assert(vm_writable(inst->arith_reg.rd)); \
    xLong * rd  = (xLong *) vm_get_reg(inst->arith_imm.rd); \
    vm_assert(rd != nullptr); \
    xShort imm  = (xShort) inst->arith_imm.imm; \
    vm_assert(the_assert); \
    write_reg(rd, expr); \
}

#define vm_execute_arith_imm_u(inst_name, expr, the_assert) \
xVoid vm_execute_##inst_name##_imm(__XVM * vm, inst * inst) { \
    vm_assert(vm_writable(inst->arith_reg.rd)); \
    xuLong * rd  = vm_get_reg(inst->arith_imm.rd); \
    vm_assert(rd != nullptr); \
    xuShort imm  = inst->arith_imm.imm; \
    vm_assert(the_assert); \
    write_reg(rd, expr); \
}

vm_execute_arith_imm_u(b_and, (*rd) & imm, true)
vm_execute_arith_imm_u(b_or, (*rd) | imm, true)
vm_execute_arith_imm_u(b_xor, (*rd) ^ imm, true)
vm_execute_arith_imm_u(b_lsh, (*rd) << imm, imm >= 0)
vm_execute_arith_imm_u(b_rsh, (*rd) >> imm, imm >= 0)
vm_execute_arith_imm_u(b_inv, !(*rd), true)

vm_execute_arith_imm_u(add_u, (*rd) + imm, true)
vm_execute_arith_imm_u(sub_u, (*rd) - imm, true)
vm_execute_arith_imm_u(mul_u, (*rd) * imm, true)
vm_execute_arith_imm_u(div_u, (*rd) / imm, imm != 0)
vm_execute_arith_imm_u(mod_u, (*rd) % imm, imm != 0)
vm_execute_arith_imm_s(add_s, (*rd) + imm, true)
vm_execute_arith_imm_s(sub_s, (*rd) - imm, true)
vm_execute_arith_imm_s(mul_s, (*rd) * imm, true)
vm_execute_arith_imm_s(div_s, (*rd) / imm, imm != 0)
vm_execute_arith_imm_s(mod_s, (*rd) % imm, imm != 0)

xVoid vm_execute_cmp_u(__XVM * vm, inst * inst) {
    xuLong *rs1 = vm_get_reg(inst->cmp_reg.rs1);
    xuLong *rs2 = vm_get_reg(inst->cmp_reg.rs2);
    if (*rs1 > *rs2) {
        vm_reg(status).fields.COMPARE_FLAG = COMPARE_FLAG_GT;
    } else if (*rs1 < *rs2) {
        vm_reg(status).fields.COMPARE_FLAG = COMPARE_FLAG_LT;
    } else {
        vm_reg(status).fields.COMPARE_FLAG = COMPARE_FLAG_EQ;
    }
}

xVoid vm_execute_cmp_s(__XVM * vm, inst * inst) {
    xLong *rs1 = (xLong *) vm_get_reg(inst->cmp_reg.rs1);
    xLong *rs2 = (xLong *) vm_get_reg(inst->cmp_reg.rs2);
    if (*rs1 > *rs2) {
        vm_reg(status).fields.COMPARE_FLAG = COMPARE_FLAG_GT;
    } else if (*rs1 < *rs2) {
        vm_reg(status).fields.COMPARE_FLAG = COMPARE_FLAG_LT;
    } else {
        vm_reg(status).fields.COMPARE_FLAG = COMPARE_FLAG_EQ;
    }
}

xVoid vm_execute_cmp_u_imm(__XVM * vm, inst * inst) {
    xuLong *rs1 = vm_get_reg(inst->cmp_reg.rs1);
    xuShort imm = (xuShort) inst->cmp_imm.imm;;
    if (*rs1 > imm) {
        vm_reg(status).fields.COMPARE_FLAG = COMPARE_FLAG_GT;
    } else if (*rs1 < imm) {
        vm_reg(status).fields.COMPARE_FLAG = COMPARE_FLAG_LT;
    } else {
        vm_reg(status).fields.COMPARE_FLAG = COMPARE_FLAG_EQ;
    }
}

xVoid vm_execute_cmp_s_imm(__XVM * vm, inst * inst) {
    xLong *rs1 = (xLong *) vm_get_reg(inst->cmp_imm.rs1);
    xShort imm = (xShort) inst->cmp_imm.imm;
    if (*rs1 > imm) {
        vm_reg(status).fields.COMPARE_FLAG = COMPARE_FLAG_GT;
    } else if (*rs1 < imm) {
        vm_reg(status).fields.COMPARE_FLAG = COMPARE_FLAG_LT;
    } else {
        vm_reg(status).fields.COMPARE_FLAG = COMPARE_FLAG_EQ;
    }
}

const static executor VM_EXECUTORS[256] = {
        [inst_nop] = vm_execute_nop,
        [inst_clear_ma_flag] = vm_execute_clear_ma_flag,
        [inst_clear_cmp_flag] = vm_execute_clear_cmp_flag,
        [inst_regex_enter] = vm_execute_enter,
        [inst_regex_reset] = vm_execute_reset,
        [inst_regex_exit] = vm_execute_exit,
        [inst_success] = vm_execute_success,
        [inst_failed] = vm_execute_failed,

        [inst_set_vm_mode] = vm_execute_set_vm_mode,
        [inst_set_ma_mode] = vm_execute_set_ma_mode,

        [inst_load] = vm_execute_load,
        [inst_store] = vm_execute_store,
        [inst_shift_move] = vm_execute_shift_move,

        [inst_seq_lit1] = vm_execute_seq_lit,
        [inst_seq_lit2] = vm_execute_seq_lit2,
        [inst_seq_lit3] = vm_execute_seq_lit3,
        [inst_set_lit1] = vm_execute_set_lit,
        [inst_set_lit2] = vm_execute_set_lit2,
        [inst_set_lit3] = vm_execute_set_lit3,
        [inst_range_lit] = vm_execute_range_lit,

        [inst_seq_reg] = vm_execute_seq_reg,
        [inst_set_reg] = vm_execute_set_reg,
        [inst_range_reg] = vm_execute_range_reg,

        [inst_jump_directly] = vm_execute_jump_directly,
        [inst_jump_if_eq] = vm_execute_jump_if_eq,
        [inst_jump_if_ne] = vm_execute_jump_if_ne,
        [inst_jump_if_lt] = vm_execute_jump_if_lt,
        [inst_jump_if_gt] = vm_execute_jump_if_gt,
        [inst_jump_if_le] = vm_execute_jump_if_le,
        [inst_jump_if_ge] = vm_execute_jump_if_ge,
        [inst_jump_if_ma] = vm_execute_jump_if_ma,
        [inst_jump_if_nm] = vm_execute_jump_if_nm,
        [inst_call] = vm_execute_call,
        [inst_ret] = vm_execute_ret,

        [inst_load_imm] = vm_execute_load_imm,

        [inst_b_and] = vm_execute_b_and,
        [inst_b_or] = vm_execute_b_or,
        [inst_b_xor] = vm_execute_b_xor,
        [inst_b_lsh] = vm_execute_b_lsh,
        [inst_b_rsh] = vm_execute_b_rsh,
        [inst_b_inv] = vm_execute_b_inv,
        [inst_add_u] = vm_execute_add_u,
        [inst_sub_u] = vm_execute_sub_u,
        [inst_mul_u] = vm_execute_mul_u,
        [inst_div_u] = vm_execute_div_u,
        [inst_mod_u] = vm_execute_mod_u,
        [inst_add_s] = vm_execute_add_s,
        [inst_sub_s] = vm_execute_sub_s,
        [inst_mul_s] = vm_execute_mul_s,
        [inst_div_s] = vm_execute_div_s,
        [inst_mod_s] = vm_execute_mod_s,

        [inst_b_and_i] = vm_execute_b_and_imm,
        [inst_b_or_i] = vm_execute_b_or_imm,
        [inst_b_xor_i] = vm_execute_b_xor_imm,
        [inst_b_lsh_i] = vm_execute_b_lsh_imm,
        [inst_b_rsh_i] = vm_execute_b_rsh_imm,
        [inst_b_inv_i] = vm_execute_b_inv_imm,
        [inst_add_u_i] = vm_execute_add_u_imm,
        [inst_sub_u_i] = vm_execute_sub_u_imm,
        [inst_mul_u_i] = vm_execute_mul_u_imm,
        [inst_div_u_i] = vm_execute_div_u_imm,
        [inst_mod_u_i] = vm_execute_mod_u_imm,
        [inst_add_s_i] = vm_execute_add_s_imm,
        [inst_sub_s_i] = vm_execute_sub_s_imm,
        [inst_mul_s_i] = vm_execute_mul_s_imm,
        [inst_div_s_i] = vm_execute_div_s_imm,
        [inst_mod_s_i] = vm_execute_mod_s_imm,

        [inst_cmp_u] = vm_execute_cmp_u,
        [inst_cmp_s] = vm_execute_cmp_s,

        [inst_cmp_u_i] = vm_execute_cmp_u_imm,
        [inst_cmp_s_i] = vm_execute_cmp_s_imm,
};
#define SET_REG_NAME(_name) [vm_##_name] = string_t(#_name)

const char_t * const VM_REG_NAMES[] = {
        SET_REG_NAME(zero_reg),
        SET_REG_NAME(reset_src_reg),
        SET_REG_NAME(ret_addr_reg),
        SET_REG_NAME(regex_level_reg),
        SET_REG_NAME(call_level_reg),
        SET_REG_NAME(status_reg),
        SET_REG_NAME(stack_reg),

        SET_REG_NAME(src_reg),
        SET_REG_NAME(inst_reg),
        SET_REG_NAME(count_reg),
        SET_REG_NAME(jump_base_reg),
        SET_REG_NAME(call_vec_reg),

        SET_REG_NAME(arith_reg_0),
        SET_REG_NAME(arith_reg_1),
        SET_REG_NAME(arith_reg_2),
        SET_REG_NAME(arith_reg_3),
        SET_REG_NAME(arith_reg_4),
        SET_REG_NAME(arith_reg_5),
        SET_REG_NAME(arith_reg_6),
        SET_REG_NAME(arith_reg_7),
        SET_REG_NAME(arith_reg_8),
        SET_REG_NAME(arith_reg_9),
        SET_REG_NAME(arith_reg_a),
        SET_REG_NAME(arith_reg_b),
        SET_REG_NAME(arith_reg_c),
        SET_REG_NAME(arith_reg_d),
        SET_REG_NAME(arith_reg_e),
        SET_REG_NAME(arith_reg_f),
};

const char_t * const VM_EXECUTE_MODE_NAMES[] = {
    [EXECUTE_REGEX_MODE] = string_t("REGEX_MODE"),
    [EXECUTE_PROCESS_MODE] = string_t("REGEX_MODE"),
};

const char_t * const VM_MATCH_MODE_NAMES[] = {
    [MATCH_NORMAL_MODE] = string_t("NORMAL_MODE"),
    [MATCH_INVERSE_MODE] = string_t("INVERSE_MODE"),
};