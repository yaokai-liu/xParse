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
xVoid vm_init(__XVM * vm, const struct Allocator* allocator) {
    vm->allocator = allocator;
    // memories init
    vm->manager = MemManager.new("Virtual Machine", allocator);
#define INST_MEM_ID         0
    MemManager.new_space(vm->manager, "INST", sizeof(inst));
#define EA_MEM_ID          1
    MemManager.new_space(vm->manager, "EA", sizeof(char_t *));
#define RA_MEM_ID           2
    MemManager.new_space(vm->manager, "RA", sizeof(xVoid *));
#define RAM_MEM_ID          3
    MemManager.new_space(vm->manager, "RAM", sizeof(xuLong));

    // register init
    vm->registers.regex_level_reg = 0;
    vm->registers.call_level_reg = 0;
    vm->registers.inst_reg = 0;
    vm->registers.ret_addr_reg = 0;
    vm->registers.src_reg = 0;
    vm->registers.status_reg.value = 0;
    vm->registers.count_reg = 0;
}

#define vm_virt2real(_ptr) MemManager.virt2real(vm->manager, _ptr)
#define vm_real2virt(_ptr) MemManager.real2virt(vm->manager, _ptr)
#define vm_raise(_error_type) do {vm->registers.status_reg.fields.TRAP_FIELD._error_type = 1; } while (false)
#define vm_writable(_reg) ( \
    (_reg) * sizeof(xuLong) >= offsetof(struct __XPARSE_VM_Registers__, src_reg) \
)
#define vm_arithmetic(_reg) ( \
 (_reg) >= __VM_ARITH_REG_START__ && (_reg) <= __VM_ARITH_REG_END__ \
)

#define vm_get_register(reg_id) ( \
((reg_id) < __VM_ARITH_REG_END__) ? ((xuLong *)&vm->registers + (reg_id)) : nullptr \
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

xVoid vm_execute_set_vm_mode(__XVM * vm, inst * inst) {
    xuByte value = 0 != (inst->set_value.value & VM_STATUS_VM_MODE_REGEXP);
    vm->registers.status_reg.fields.MODE_FIELD.vm_mode = value;
}

xVoid vm_execute_set_ma_mode(__XVM * vm, inst * inst) {
    xuByte value = 0 != (inst->set_value.value & VM_STATUS_VM_MODE_REGEXP);
    vm->registers.status_reg.fields.MODE_FIELD.ma_mode = value;
}

xVoid vm_execute_clear_ma_flag(__XVM * vm, inst * inst) {
    * (xuByte *) &(vm->registers.status_reg.fields.FLAG_FIELD) = 0;
}

xVoid vm_execute_clear_cmp_flag(__XVM * vm, inst * inst) {
    * (xuByte *) &(vm->registers.status_reg.fields.FLAG_FIELD) = 0;
}

#define FLAG_MATCHED        0b0100'0000
#define FLAG_NOT_MATCHED    0b1000'0000
xVoid vm_execute_seq_lit(__XVM * vm, inst * inst) {
    if (vm->registers.src_reg[0] == inst->match_lit.target[0]) {
        vm->registers.src_reg ++;
        vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_MATCHED;
    } else {
        vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_NOT_MATCHED;
    }
}

xVoid vm_execute_seq_lit2(__XVM * vm, inst * inst) {
    if (vm->registers.src_reg[0] == inst->match_lit.target[0]
      &&vm->registers.src_reg[1] == inst->match_lit.target[1]) {
        vm->registers.src_reg += 2;
        vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_MATCHED;
    } else {
        vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_NOT_MATCHED;
    }
}

xVoid vm_execute_seq_lit3(__XVM * vm, inst * inst) {
    if (vm->registers.src_reg[0] == inst->match_lit.target[0]
      &&vm->registers.src_reg[1] == inst->match_lit.target[1]
      &&vm->registers.src_reg[2] == inst->match_lit.target[2]) {
        vm->registers.src_reg += 3;
        vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_MATCHED;
    } else {
        vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_NOT_MATCHED;
    }
}

xVoid vm_execute_set_lit(__XVM * vm, inst * inst) {
    if (vm->registers.src_reg[0] == inst->match_lit.target[0]) {
        vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_MATCHED;
    } else {
        vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_NOT_MATCHED;
    }
}

xVoid vm_execute_set_lit2(__XVM * vm, inst * inst) {
    if (vm->registers.src_reg[0] == inst->match_lit.target[0]
      ||vm->registers.src_reg[0] == inst->match_lit.target[1]) {
        vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_MATCHED;
    } else {
        vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_NOT_MATCHED;
    }
}

xVoid vm_execute_set_lit3(__XVM * vm, inst * inst) {
    if (vm->registers.src_reg[0] == inst->match_lit.target[0]
      ||vm->registers.src_reg[0] == inst->match_lit.target[1]
      ||vm->registers.src_reg[0] == inst->match_lit.target[2]) {
        vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_MATCHED;
    } else {
        vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_NOT_MATCHED;
    }
}

xVoid vm_execute_range_lit(__XVM * vm, inst * inst) {
    if (*vm->registers.src_reg >= inst->match_lit.target[0]
      &&*vm->registers.src_reg <= inst->match_lit.target[1]) {
        vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_MATCHED;
    } else {
        vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_NOT_MATCHED;
    }
}

xVoid vm_execute_seq_reg(__XVM * vm, inst * inst) {
    char_t ** target = (char_t **) vm_get_register(vm, inst->match_reg.reg);
    vm_assert(target != nullptr);
    xBool _b = strcmp_i(vm->registers.src_reg, *target, vm->registers.count_reg);
    vm->registers.src_reg += vm->registers.count_reg;
    vm->registers.status_reg.bytes[FLAG_FILED_IDX] = _b ? FLAG_MATCHED : FLAG_NOT_MATCHED;
    vm->registers.count_reg = 0;
}

xVoid vm_execute_set_reg(__XVM * vm, inst * inst) {
    char_t ** target = (char_t **) vm_get_register(vm, inst->match_reg.reg);
    vm_assert(target != nullptr);
    xInt _b = stridx_i(*target, *vm->registers.src_reg, vm->registers.count_reg);
    vm->registers.status_reg.bytes[FLAG_FILED_IDX] = _b ? FLAG_MATCHED : FLAG_NOT_MATCHED;
    vm->registers.count_reg = 0;
}

xVoid vm_execute_range_reg(__XVM * vm, inst * inst) {
    struct range { char_t start; char_t end; };
    struct range ** target = (struct range **) vm_get_register(inst->match_reg.reg);
    vm_assert(target != nullptr);
    for (;vm->registers.count_reg > 0; vm->registers.count_reg --) {
        if (target[vm->registers.count_reg - 1]->start <= *vm->registers.src_reg
           && target[vm->registers.count_reg - 1]->end >= *vm->registers.src_reg) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_MATCHED;
            vm->registers.count_reg = 0;
            return;
        }
    }
    vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_NOT_MATCHED;
}

xVoid vm_execute_enter(__XVM * vm, inst * inst) {
    mem_space * space = MemManager.get_space(vm->manager, EA_MEM_ID);
    MemSpace.push(space, &(vm->registers.reset_src_reg));
    vm->registers.reset_src_reg = vm->registers.src_reg;
    vm->registers.regex_level_reg ++;
}

xVoid vm_execute_reset(__XVM * vm, inst * inst) {
    vm->registers.src_reg = vm->registers.reset_src_reg;
}

xVoid vm_execute_exit(__XVM * vm, inst * inst) {
    if (vm->registers.regex_level_reg == 0) { vm_raise(regex_level_error); return ; }
    mem_space * space = MemManager.get_space(vm->manager, EA_MEM_ID);
    MemSpace.pop(space, &(vm->registers.reset_src_reg));
    vm->registers.regex_level_reg --;
}

#define jump_to(_offset)   do { vm->registers.inst_reg = vm->registers.jump_base_reg + _offset; } while (false)

xVoid vm_execute_jump_directly(__XVM * vm, inst * inst) {
    jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_eq(__XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.eq_flag)
        jump_to(inst->jump.offset);

}

xVoid vm_execute_jump_if_ne(__XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.ne_flag)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_gt(__XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.gt_flag)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_lt(__XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.lt_flag)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_ge(__XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.gt_flag
      ||vm->registers.status_reg.fields.FLAG_FIELD.eq_flag)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_le(__XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.lt_flag
      ||vm->registers.status_reg.fields.FLAG_FIELD.eq_flag)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_ma(__XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.ma_flag)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_jump_if_nm(__XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.nm_flag)
        jump_to(inst->jump.offset);
}

xVoid vm_execute_call(__XVM * vm, inst * inst) {
    mem_space * space = MemManager.get_space(vm->manager, RA_MEM_ID);
    MemSpace.push(space, &vm->registers.ret_addr_reg);
    vm->registers.ret_addr_reg = vm->registers.inst_reg + 1;
    vm->registers.inst_reg = vm->registers.call_vec_reg[inst->jump.offset];
    vm->registers.call_level_reg ++;
}

xVoid vm_execute_ret(__XVM * vm, inst * inst) {
    vm->registers.inst_reg = vm->registers.ret_addr_reg;
    mem_space * space = MemManager.get_space(vm->manager, RA_MEM_ID);
    MemSpace.pop(space, &(vm->registers.ret_addr_reg));
    vm->registers.call_level_reg --;
}

#define write_reg(_rd, _value) do {*(_rd) = (_rd) ? (_value) : 0;} while (false)
#define vm_assert(_the_bool) do { if (!(_the_bool)) {vm_raise(illegal_inst); return; } } while (false)

xVoid vm_execute_load(__XVM * vm, inst * inst) {
    vm_assert(vm_writable(inst->msl_reg.rd));
    xuLong * rd = vm_get_register(inst->msl_reg.rd);
    xuLong * rs = vm_get_register(inst->msl_reg.rs);
    rs = vm_virt2real(* (xVoid **) rs);
    write_reg(rd, ((xuLong *)rs)[inst->msl_reg.offset]);
}

xVoid vm_execute_store(__XVM * vm, inst * inst) {
    xuLong * rd = vm_get_register(inst->msl_reg.rd);
    xuLong * rs = vm_get_register(inst->msl_reg.rs);
    rd = vm_virt2real(* (xVoid **) rd);
    vm_assert(rd != nullptr);
    ((xuLong *)rd)[inst->msl_reg.offset] = * rs;
}

xVoid vm_execute_shift_move(__XVM * vm, inst * inst) {
    vm_assert(vm_writable(inst->msl_reg.rd));
    xuLong * rd = vm_get_register(inst->msl_reg.rd);
    xuLong * rs = vm_get_register(inst->msl_reg.rs);
    vm_assert(rd != nullptr && rs != nullptr);
    write_reg(rd, (*rs) << (inst->msl_reg.offset));
}

xVoid vm_execute_load_imm(__XVM * vm, inst * inst) {
    vm_assert(vm_arithmetic(inst->msl_reg.rd));
    xuLong * rd = vm_get_register(inst->load_imm.rd);
    vm_assert(rd != nullptr);
    write_reg(rd, inst->load_imm.imm);
}

#define vm_execute_arith(inst_name, expr, the_assert) \
xVoid vm_execute_##inst_name(__XVM * vm, inst * inst) { \
    vm_assert(vm_arithmetic(inst->msl_reg.rd)); \
    if (!inst->arith.is_signed) { \
        xuLong * rd  = vm_get_register(inst->arith.rd); \
        xuLong * rs1 = vm_get_register(inst->arith.rs1); \
        xuLong * rs2 = vm_get_register(inst->arith.rs2); \
        vm_assert(rd != nullptr && rs1 != nullptr && rs2 != nullptr); \
        vm_assert(the_assert); \
        write_reg(rd, expr); \
    } else { \
        xLong * rd  = (xLong *) vm_get_register(inst->arith.rd); \
        xLong * rs1 = (xLong *) vm_get_register(inst->arith.rs1); \
        xLong * rs2 = (xLong *) vm_get_register(inst->arith.rs2); \
        vm_assert(rd != nullptr && rs1 != nullptr && rs2 != nullptr); \
        vm_assert(the_assert); \
        write_reg(rd, expr); \
    } \
}

vm_execute_arith(add, (*rs1) + (*rs2), true)
vm_execute_arith(sub, (*rs1) - (*rs2), true)
vm_execute_arith(mul, (*rs1) * (*rs2), true)
vm_execute_arith(div, (*rs1) / (*rs2), (*rs2) != 0)
vm_execute_arith(mod, (*rs1) % (*rs2), (*rs2) != 0)
vm_execute_arith(b_and, (*rs1) & (*rs2), true)
vm_execute_arith(b_or, (*rs1) | (*rs2), true)
vm_execute_arith(b_xor, (*rs1) ^ (*rs2), true)
vm_execute_arith(b_lsh, (*rs1) << (*rs2), true)
vm_execute_arith(b_rsh, (*rs1) >> (*rs2), true)
vm_execute_arith(b_inv, !(*rs1), true)

#define vm_execute_arith_imm(inst_name, expr, the_assert) \
xVoid vm_execute_##inst_name##_imm(__XVM * vm, inst * inst) { \
    vm_assert(vm_arithmetic(inst->msl_reg.rd)); \
    if (!inst->arith.is_signed) { \
        xuLong * rd  = vm_get_register(inst->arith.rd); \
        xuLong * rs1 = vm_get_register(inst->arith.rs1); \
        vm_assert(rd != nullptr && rs1 != nullptr); \
        xuShort imm  = inst->arith_imm.imm; \
        vm_assert(the_assert); \
        write_reg(rd, expr); \
    } else { \
        xLong * rd  = (xLong *) vm_get_register(inst->arith.rd); \
        xLong * rs1 = (xLong *) vm_get_register(inst->arith.rs1); \
        vm_assert(rd != nullptr && rs1 != nullptr); \
        xShort imm  = (xShort) inst->arith_imm.imm; \
        vm_assert(the_assert); \
        write_reg(rd, expr); \
    } \
}

vm_execute_arith_imm(add, (*rs1) + imm, true)
vm_execute_arith_imm(sub, (*rs1) - imm, true)
vm_execute_arith_imm(mul, (*rs1) * imm, true)
vm_execute_arith_imm(div, (*rs1) / imm, imm != 0)
vm_execute_arith_imm(mod, (*rs1) % imm, imm != 0)
vm_execute_arith_imm(b_and, (*rs1) & imm, true)
vm_execute_arith_imm(b_or, (*rs1) | imm, true)
vm_execute_arith_imm(b_xor, (*rs1) ^ imm, true)
vm_execute_arith_imm(b_lsh, (*rs1) << imm, imm >= 0)
vm_execute_arith_imm(b_rsh, (*rs1) >> imm, imm >= 0)
vm_execute_arith_imm(b_inv, !(*rs1), true)


#define FLAG_GT     0b0000'1010
#define FLAG_LT     0b0000'0110
#define FLAG_EQ     0b0000'0001

xVoid vm_execute_cmp(__XVM * vm, inst * inst) {
    if (inst->cmp_reg.is_signed) {
        xLong *rs1 = (xLong *) vm_get_register(inst->cmp_reg.rs1);
        xLong *rs2 = (xLong *) vm_get_register(inst->cmp_reg.rs2);
        if (*rs1 > *rs2) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_GT;
        } else if (*rs1 < *rs2) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_LT;
        } else {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_EQ;
        }
    } else {
        xuLong *rs1 = vm_get_register(inst->cmp_reg.rs1);
        xuLong *rs2 = vm_get_register(inst->cmp_reg.rs2);
        if (*rs1 > *rs2) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_GT;
        } else if (*rs1 < *rs2) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_LT;
        } else {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_EQ;
        }
    }
}

xVoid vm_execute_cmp_imm(__XVM * vm, inst * inst) {
    if (inst->cmp_imm.is_signed) {
        xLong *rs1 = (xLong *) vm_get_register(inst->cmp_imm.rs1);
        xShort imm = (xShort) inst->cmp_imm.imm;
        if (*rs1 > imm) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_GT;
        } else if (*rs1 < imm) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_LT;
        } else {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_EQ;
        }
    } else {
        xuLong *rs1 = vm_get_register(inst->cmp_reg.rs1);
        xuShort imm = (xuShort) inst->cmp_imm.imm;;
        if (*rs1 > imm) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_GT;
        } else if (*rs1 > imm) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_LT;
        } else {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_EQ;
        }
    }
}

const static executor VM_EXECUTORS[256] = {
        [inst_nop] = vm_execute_nop,

        [inst_set_vm_mode] = vm_execute_set_vm_mode,
        [inst_set_ma_mode] = vm_execute_set_ma_mode,
        [inst_clear_ma_flag] = vm_execute_clear_ma_flag,
        [inst_clear_cmp_flag] = vm_execute_clear_cmp_flag,

        [inst_load] = vm_execute_load,
        [inst_store] = vm_execute_store,
        [inst_shift_move] = vm_execute_shift_move,
        [inst_load_imm] = vm_execute_load_imm,

        [inst_seq_lit1] = vm_execute_seq_lit,
        [inst_seq_lit2] = vm_execute_seq_lit2,
        [inst_seq_lit3] = vm_execute_seq_lit3,
        [inst_set_lit1] = vm_execute_set_lit,
        [inst_set_lit2] = vm_execute_set_lit2,
        [inst_set_lit3] = vm_execute_set_lit3,
        [inst_seq_reg] = vm_execute_seq_reg,
        [inst_set_reg] = vm_execute_set_reg,
        [inst_range_reg] = vm_execute_range_reg,
        [inst_range_lit] = vm_execute_range_lit,

        [inst_ctx_enter] = vm_execute_enter,
        [inst_ctx_reset] = vm_execute_reset,
        [inst_ctx_exit] = vm_execute_exit,

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
        [inst_success] = vm_execute_success,
        [inst_failed] = vm_execute_failed,

        [inst_add] = vm_execute_add,
        [inst_sub] = vm_execute_sub,
        [inst_mul] = vm_execute_mul,
        [inst_div] = vm_execute_div,
        [inst_mod] = vm_execute_mod,
        [inst_b_and] = vm_execute_b_and,
        [inst_b_or] = vm_execute_b_or,
        [inst_b_xor] = vm_execute_b_xor,
        [inst_b_lsh] = vm_execute_b_lsh,
        [inst_b_rsh] = vm_execute_b_rsh,
        [inst_b_inv] = vm_execute_b_inv,
        [inst_cmp] = vm_execute_cmp,
        [inst_add_i] = vm_execute_add_imm,
        [inst_sub_i] = vm_execute_sub_imm,
        [inst_mul_i] = vm_execute_mul_imm,
        [inst_div_i] = vm_execute_div_imm,
        [inst_mod_i] = vm_execute_mod_imm,
        [inst_b_and_i] = vm_execute_b_and_imm,
        [inst_b_or_i] = vm_execute_b_or_imm,
        [inst_b_xor_i] = vm_execute_b_xor_imm,
        [inst_b_lsh_i] = vm_execute_b_lsh_imm,
        [inst_b_rsh_i] = vm_execute_b_rsh_imm,
        [inst_b_inv_i] = vm_execute_b_inv_imm,
        [inst_cmp_i] = vm_execute_cmp_imm,
};
