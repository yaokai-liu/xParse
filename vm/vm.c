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

struct status_reg {
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define MODE_FILED_IDX  2
#define FLAG_FILED_IDX  3
#define TRAP_FILED_IDX  5
    xuByte __reserved_1__[2]; // NOLINT(*-reserved-identifier)
    struct {
        xuByte  : 3;
        xuByte  ma_mode: 1;
        xuByte  : 3;
        xuByte  vm_mode: 1;
    }   MODE_FIELD;
    struct {
        xuByte  eq_flag: 1;
        xuByte  ne_flag: 1;
        xuByte  lt_flag: 1;
        xuByte  gt_flag: 1;
        xuByte  : 2;
        xuByte  ma_flag: 1;
        xuByte  nm_flag: 1;
    }   FLAG_FIELD;
    xuByte __reserved_2__[1]; // NOLINT(*-reserved-identifier)
    struct {
        xuByte  : 3;
        xuByte  breakpoint: 1;
        xuByte  : 1;
        xuByte  miss_align_inst: 1;
        xuByte  illegal_inst: 1;
        xuByte  div_zero: 1;
    } TRAP_FIELD;
    xuByte __reserved_3__[2]; // NOLINT(*-reserved-identifier)
#elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define MODE_FILED_IDX  5
#define FLAG_FILED_IDX  4
#define TRAP_FILED_IDX  2
    xuByte __reserved_1__[2]; // NOLINT(*-reserved-identifier)
    struct {
        xuByte  div_zero: 1;
        xuByte  illegal_inst: 1;
        xuByte  miss_align_inst: 1;
        xuByte  : 1;
        xuByte  breakpoint: 1;
        xuByte  : 3;
    } TRAP_FIELD;
    xuByte __reserved_2__[1]; // NOLINT(*-reserved-identifier)
    struct {
        xuByte  nm_flag: 1;
        xuByte  ma_flag: 1;
        xuByte  : 2;
        xuByte  gt_flag: 1;
        xuByte  lt_flag: 1;
        xuByte  ne_flag: 1;
        xuByte  eq_flag: 1;
    }   FLAG_FIELD;
    struct {
        xuByte  vm_mode: 1;
        xuByte  : 3;
        xuByte  ma_mode: 1;
        xuByte  : 3;
    }   MODE_FIELD;
    xuByte __reserved_3__[2]; // NOLINT(*-reserved-identifier)
#elif (__BYTE_ORDER__ == __ORDER_PDP_ENDIAN__)
#error "Unsupported endian: PDP ENDIAN."
#else
#error "No endian information found."
#endif
};



struct __XPARSE_VM_Registers__ { // NOLINT(*-reserved-identifier)
    // read only registers
    const xuLong    zero_reg        [[gnu::aligned(sizeof(xuLong))]];    // zero
    char_t *        src_reg         [[gnu::aligned(sizeof(xuLong))]];    // sc
    inst *          inst_reg        [[gnu::aligned(sizeof(xuLong))]];    // pc
    inst *          ret_addr_reg    [[gnu::aligned(sizeof(xuLong))]];    // ra
    xuLong          reg_level_reg   [[gnu::aligned(sizeof(xuLong))]];    // src stack top
    xuLong          call_level_reg  [[gnu::aligned(sizeof(xuLong))]];    // ra stack top
    union {
        xuLong  value;
        xuByte  bytes[8];
        struct status_reg fields;
    }               status_reg      [[gnu::aligned(sizeof(xuLong))]];    // status
    xuLong          stack_reg       [[gnu::aligned(sizeof(xuLong))]];    // stack top

    // read & write registers
    xuLong          count_reg       [[gnu::aligned(sizeof(xuLong))]];    // cr
    inst *          jump_base_reg   [[gnu::aligned(sizeof(xuLong))]];

    // arithmetic registers
    xuLong          arith_reg[15];
};


#define XVM __XPARSE_VirtualMachine__
struct XVM { // NOLINT(*-reserved-identifier)
    // registers
    struct __XPARSE_VM_Registers__ registers;

    // memories
    mem_manager *     manager;
    struct Allocator* allocator;
};

xVoid vm_init(struct XVM * vm);
xVoid vm_execute(struct XVM * vm);

static struct __XPARSE_VM_Method__ VM = {
        .init = vm_init,
        .execute = vm_execute,
};

xVoid vm_init(struct XVM * vm) {
    // memories init
    vm->manager = MemManager.new("Virtual Machine", vm->allocator);
#define INST_MEM_ID         0
    MemManager.new_space(vm->manager, "INST", sizeof(inst));
#define SRC_MEM_ID          1
    MemManager.new_space(vm->manager, "SRC", sizeof(char_t *));
#define RA_MEM_ID           2
    MemManager.new_space(vm->manager, "RA", sizeof(xVoid *));
#define RAM_MEM_ID          3
    MemManager.new_space(vm->manager, "RAM", sizeof(xuLong));

    // register init
    vm->registers.reg_level_reg = 0;
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
#define vm_writable(_reg) ((_reg) * sizeof(xuLong) >= offsetof(struct __XPARSE_VM_Registers__, count_reg))

inline xuLong * vm_get_register(struct XVM * vm, xuByte reg_id) {
    if (reg_id < sizeof(typeof(vm->registers)) / sizeof(xuLong)) {
        return ((xuLong *)&vm->registers + reg_id);
    }
    vm_raise(illegal_inst);
    return nullptr;
}

typedef xVoid (*executor)(struct XVM * vm, inst * inst);
const static executor VM_EXECUTORS[256];

xVoid vm_execute(struct XVM * vm) {
    union inst * instruction = vm_virt2real(vm->registers.inst_reg);
    VM_EXECUTORS[instruction->single.opcode](vm, instruction);
    vm->registers.inst_reg ++;
}

inline xVoid vm_execute_nop(struct XVM * vm, inst * inst) {
}

inline xVoid vm_execute_set_vm_mode(struct XVM * vm, inst * inst) {
    xuByte value = 0 != (inst->set_value.value & VM_SET_STATUS_VM_MODE_REGEXP);
    vm->registers.status_reg.fields.MODE_FIELD.vm_mode = value;
}

inline xVoid vm_execute_set_ma_mode(struct XVM * vm, inst * inst) {
    xuByte value = 0 != (inst->set_value.value & VM_SET_STATUS_VM_MODE_REGEXP);
    vm->registers.status_reg.fields.MODE_FIELD.ma_mode = value;
}

inline xVoid vm_execute_clear_ma_flag(struct XVM * vm, inst * inst) {
    * (xuByte *) &(vm->registers.status_reg.fields.FLAG_FIELD) = 0;
}

inline xVoid vm_execute_clear_cmp_flag(struct XVM * vm, inst * inst) {
    * (xuByte *) &(vm->registers.status_reg.fields.FLAG_FIELD) = 0;
}

inline xVoid vm_execute_seq_lit(struct XVM * vm, inst * inst) {
    if (vm->registers.src_reg[0] == inst->match_lit.target[0]) {
        vm->registers.src_reg ++;
        vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = 1;
        vm->registers.status_reg.fields.FLAG_FIELD.nm_flag = 0;
    } else {
        vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = 0;
        vm->registers.status_reg.fields.FLAG_FIELD.nm_flag = 1;
    }
}

inline xVoid vm_execute_seq_lit2(struct XVM * vm, inst * inst) {
    if (vm->registers.src_reg[0] == inst->match_lit.target[0]
      &&vm->registers.src_reg[1] == inst->match_lit.target[1]) {
        vm->registers.src_reg += 2;
        vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = 1;
        vm->registers.status_reg.fields.FLAG_FIELD.nm_flag = 0;
    } else {
        vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = 0;
        vm->registers.status_reg.fields.FLAG_FIELD.nm_flag = 1;
    }
}

inline xVoid vm_execute_seq_lit3(struct XVM * vm, inst * inst) {
    if (vm->registers.src_reg[0] == inst->match_lit.target[0]
      &&vm->registers.src_reg[1] == inst->match_lit.target[1]
      &&vm->registers.src_reg[2] == inst->match_lit.target[2]) {
        vm->registers.src_reg += 3;
        vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = 1;
        vm->registers.status_reg.fields.FLAG_FIELD.nm_flag = 0;
    } else {
        vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = 0;
        vm->registers.status_reg.fields.FLAG_FIELD.nm_flag = 1;
    }
}

inline xVoid vm_execute_set_lit(struct XVM * vm, inst * inst) {
    if (vm->registers.src_reg[0] == inst->match_lit.target[0]) {
        vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = 1;
        vm->registers.status_reg.fields.FLAG_FIELD.nm_flag = 0;
    } else {
        vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = 0;
        vm->registers.status_reg.fields.FLAG_FIELD.nm_flag = 1;
    }
}

inline xVoid vm_execute_set_lit2(struct XVM * vm, inst * inst) {
    if (vm->registers.src_reg[0] == inst->match_lit.target[0]
      ||vm->registers.src_reg[0] == inst->match_lit.target[1]) {
        vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = 1;
        vm->registers.status_reg.fields.FLAG_FIELD.nm_flag = 0;
    } else {
        vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = 0;
        vm->registers.status_reg.fields.FLAG_FIELD.nm_flag = 1;
    }
}

inline xVoid vm_execute_set_lit3(struct XVM * vm, inst * inst) {
    if (vm->registers.src_reg[0] == inst->match_lit.target[0]
      ||vm->registers.src_reg[0] == inst->match_lit.target[1]
      ||vm->registers.src_reg[0] == inst->match_lit.target[2]) {
        vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = 1;
        vm->registers.status_reg.fields.FLAG_FIELD.nm_flag = 0;
    } else {
        vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = 0;
        vm->registers.status_reg.fields.FLAG_FIELD.nm_flag = 1;
    }
}

#define FLAG_MATCHED        0b0100'0000
#define FLAG_NOT_MATCHED    0b1000'0000
inline xVoid vm_execute_seq_reg(struct XVM * vm, inst * inst) {
    char_t ** target = (char_t **) vm_get_register(vm, inst->match_reg.reg);
    xBool _b = strcmp_i(vm->registers.src_reg, *target, vm->registers.count_reg);
    vm->registers.src_reg += _b;
    vm->registers.status_reg.bytes[FLAG_FILED_IDX] = _b ? FLAG_MATCHED : FLAG_NOT_MATCHED;
}

inline xVoid vm_execute_set_reg(struct XVM * vm, inst * inst) {
    char_t ** target = (char_t **) vm_get_register(vm, inst->match_reg.reg);
    xInt _b = stridx_i(*target, *vm->registers.src_reg, vm->registers.count_reg);
    vm->registers.status_reg.bytes[FLAG_FILED_IDX] = _b ? FLAG_MATCHED : FLAG_NOT_MATCHED;

}

inline xVoid vm_execute_enter(struct XVM * vm, inst * inst) {
    mem_space * space = MemManager.get_space(vm->manager, SRC_MEM_ID);
    MemSpace.push(space, &(vm->registers.src_reg));
    vm->registers.reg_level_reg = MemSpace.size(space) - 1;
}

inline xVoid vm_execute_reset(struct XVM * vm, inst * inst) {
    mem_space * space = MemManager.get_space(vm->manager, SRC_MEM_ID);
    vm->registers.src_reg = *(char_t **)MemSpace.real_addr(space, vm->registers.reg_level_reg);
}

inline xVoid vm_execute_exit(struct XVM * vm, inst * inst) {
    mem_space * space = MemManager.get_space(vm->manager, SRC_MEM_ID);
    MemSpace.pop(space, nullptr);
    vm->registers.reg_level_reg = MemSpace.size(space) - 1;
}

#define jump_to(_offset)   do { vm->registers.inst_reg = vm->registers.jump_base_reg + _offset; } while (false)

inline xVoid vm_execute_jump_directly(struct XVM * vm, inst * inst) {
    jump_to(inst->jump.offset);
}

inline xVoid vm_execute_jump_if_eq(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.eq_flag)
        jump_to(inst->jump.offset);

}

inline xVoid vm_execute_jump_if_ne(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.ne_flag)
        jump_to(inst->jump.offset);
}

inline xVoid vm_execute_jump_if_gt(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.gt_flag)
        jump_to(inst->jump.offset);
}

inline xVoid vm_execute_jump_if_lt(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.lt_flag)
        jump_to(inst->jump.offset);
}

inline xVoid vm_execute_jump_if_ge(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.gt_flag
      ||vm->registers.status_reg.fields.FLAG_FIELD.eq_flag)
        jump_to(inst->jump.offset);
}

inline xVoid vm_execute_jump_if_le(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.lt_flag
      ||vm->registers.status_reg.fields.FLAG_FIELD.eq_flag)
        jump_to(inst->jump.offset);
}

inline xVoid vm_execute_jump_if_ma(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.ma_flag)
        jump_to(inst->jump.offset);
}

inline xVoid vm_execute_jump_if_nm(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.nm_flag)
        jump_to(inst->jump.offset);
}

inline xVoid vm_execute_call(struct XVM * vm, inst * inst) {
    mem_space * space = MemManager.get_space(vm->manager, RA_MEM_ID);
    MemSpace.push(space, &vm->registers.ret_addr_reg);
    vm->registers.ret_addr_reg = vm->registers.inst_reg + 1;
    vm->registers.call_level_reg = MemSpace.size(space);
    jump_to(inst->jump.offset);
}

inline xVoid vm_execute_ret(struct XVM * vm, inst * inst) {
    vm->registers.inst_reg = vm->registers.ret_addr_reg;
    mem_space * space = MemManager.get_space(vm->manager, RA_MEM_ID);
    MemSpace.pop(space, &vm->registers.ret_addr_reg);
    vm->registers.call_level_reg = MemSpace.size(space);
}

#define write_reg(_rd, _value) do {*(_rd) = (_rd) ? (_value) : 0;} while (false)
#define vm_assert(_the_bool) do { if (!(_the_bool)) {vm_raise(illegal_inst); return; } } while (false)

inline xVoid vm_execute_load(struct XVM * vm, inst * inst) {
    vm_assert(vm_writable(inst->msl_reg.rd));
    xuLong * rd = vm_get_register(vm, inst->msl_reg.rd);
    xuLong * rs = vm_get_register(vm, inst->msl_reg.rs);
    rd = vm_virt2real(rd);
    rs = vm_virt2real(rs);
    write_reg(rd, ((xuLong *)rs)[inst->msl_reg.offset]);
}

inline xVoid vm_execute_load_imm(struct XVM * vm, inst * inst) {
    vm_assert(vm_writable(inst->msl_reg.rd));
    xuLong * rd = vm_get_register(vm, inst->load_imm.rd);
    write_reg(rd, inst->load_imm.imm);
}

#define vm_execute_arith(inst_name, expr, the_assert) \
inline xVoid vm_execute_##inst_name(struct XVM * vm, inst * inst) { \
    vm_assert(vm_writable(inst->msl_reg.rd)); \
    if (!inst->arith.is_signed) { \
        xuLong * rd  = vm_get_register(vm, inst->arith.rd); \
        xuLong * rs1 = vm_get_register(vm, inst->arith.rs1); \
        xuLong * rs2 = vm_get_register(vm, inst->arith.rs2); \
        vm_assert(the_assert); \
        write_reg(rd, expr); \
    } else { \
        xLong * rd  = (xLong *) vm_get_register(vm, inst->arith.rd); \
        xLong * rs1 = (xLong *) vm_get_register(vm, inst->arith.rs1); \
        xLong * rs2 = (xLong *) vm_get_register(vm, inst->arith.rs2); \
        vm_assert(the_assert); \
        write_reg(rd, expr); \
    } \
}

vm_execute_arith(add, *rs1 + *rs2, true)
vm_execute_arith(sub, *rs1 - *rs2, true)
vm_execute_arith(mul, *rs1 * *rs2, true)
vm_execute_arith(div, *rs1 / *rs2, *rs2 != 0)
vm_execute_arith(mod, *rs1 % *rs2, *rs2 != 0)
vm_execute_arith(b_and, *rs1 & *rs2, true)
vm_execute_arith(b_or, *rs1 | *rs2, true)
vm_execute_arith(b_xor, *rs1 ^ *rs2, true)
vm_execute_arith(b_lsh, *rs1 << *rs2, true)
vm_execute_arith(b_rsh, *rs1 >> *rs2, true)
vm_execute_arith(b_inv, !*rs1, true)

#define vm_execute_arith_imm(inst_name, expr, the_assert) \
inline xVoid vm_execute_##inst_name##_imm(struct XVM * vm, inst * inst) { \
    vm_assert(vm_writable(inst->msl_reg.rd)); \
    if (!inst->arith.is_signed) { \
        xuLong * rd  = vm_get_register(vm, inst->arith.rd); \
        xuLong * rs1 = vm_get_register(vm, inst->arith.rs1); \
        xuShort imm  = inst->arith_imm.imm; \
        vm_assert(the_assert); \
        write_reg(rd, expr); \
    } else { \
        xLong * rd  = (xLong *) vm_get_register(vm, inst->arith.rd); \
        xLong * rs1 = (xLong *) vm_get_register(vm, inst->arith.rs1); \
        xShort imm  = (xShort) inst->arith_imm.imm; \
        vm_assert(the_assert); \
        write_reg(rd, expr); \
    } \
}

vm_execute_arith_imm(add, *rs1 + imm, true)
vm_execute_arith_imm(sub, *rs1 - imm, true)
vm_execute_arith_imm(mul, *rs1 * imm, true)
vm_execute_arith_imm(div, *rs1 / imm, imm != 0)
vm_execute_arith_imm(mod, *rs1 % imm, imm != 0)
vm_execute_arith_imm(b_and, *rs1 & imm, true)
vm_execute_arith_imm(b_or, *rs1 | imm, true)
vm_execute_arith_imm(b_xor, *rs1 ^ imm, true)
vm_execute_arith_imm(b_lsh, *rs1 << imm, imm >= 0)
vm_execute_arith_imm(b_rsh, *rs1 >> imm, imm >= 0)
vm_execute_arith_imm(b_inv, !*rs1, true)


#define FLAG_GT     0b0000'1010
#define FLAG_LT     0b0000'0110
#define FLAG_EQ     0b0000'0001
inline xVoid vm_execute_cmp(struct XVM * vm, inst * inst) {
    if (inst->cmp_reg.is_signed) {
        xLong *rs1 = (xLong *) vm_get_register(vm, inst->cmp_reg.rs1);
        xLong *rs2 = (xLong *) vm_get_register(vm, inst->cmp_reg.rs2);
        if (*rs1 > *rs2) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_GT;
        } else if (*rs1 < *rs2) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_LT;
        } else {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_EQ;
        }
    } else {
        xuLong *rs1 = vm_get_register(vm, inst->cmp_reg.rs1);
        xuLong *rs2 = vm_get_register(vm, inst->cmp_reg.rs2);
        if (*rs1 > *rs2) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_GT;
        } else if (*rs1 < *rs2) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_LT;
        } else {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_EQ;
        }
    }
}

inline xVoid vm_execute_cmp_imm(struct XVM * vm, inst * inst) {
    if (inst->cmp_imm.is_signed) {
        xLong *rs1 = (xLong *) vm_get_register(vm, inst->cmp_imm.rs1);
        xShort imm = (xShort) inst->cmp_imm.imm;
        if (*rs1 > imm) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_GT;
        } else if (*rs1 < imm) {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_LT;
        } else {
            vm->registers.status_reg.bytes[FLAG_FILED_IDX] = FLAG_EQ;
        }
    } else {
        xuLong *rs1 = vm_get_register(vm, inst->cmp_reg.rs1);
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

