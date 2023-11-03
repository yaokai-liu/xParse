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

struct status_reg {
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
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
        struct status_reg fields;
    }               status_reg      [[gnu::aligned(sizeof(xuLong))]];    // status
    xuLong          stack_reg       [[gnu::aligned(sizeof(xuLong))]];    // stack top
    // read & write registers
    xuLong          count_erg       [[gnu::aligned(sizeof(xuLong))]];    // cr

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
    vm->registers.count_erg = 0;
}

inline xuLong * vm_get_register(struct XVM * vm, xuByte reg_id) {
    if (reg_id < sizeof(typeof(vm->registers)) / sizeof(xuLong)) {
        return ((xuLong *)&vm->registers + reg_id);
    }
    vm->registers.status_reg.fields.TRAP_FIELD.illegal_inst = 1;
    return nullptr;
}

typedef xVoid (*executor)(struct XVM * vm, inst * inst);
const static executor VM_EXECUTORS[256];

#define vm_virt2real(_ptr) MemManager.virt2real(vm->manager, _ptr)
#define vm_real2virt(_ptr) MemManager.real2virt(vm->manager, _ptr)

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

inline xVoid vm_execute_seq_reg(struct XVM * vm, inst * inst) {
    char_t ** target = (char_t **) vm_get_register(vm, inst->match_reg.reg);
    xBool _b = strcmp_i(vm->registers.src_reg, *target, vm->registers.count_erg);
    vm->registers.src_reg += _b;
    vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = _b;
    vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = !_b;
}

inline xVoid vm_execute_set_reg(struct XVM * vm, inst * inst) {
    char_t ** target = (char_t **) vm_get_register(vm, inst->match_reg.reg);
    xInt _b = stridx_i(*target, vm->registers.src_reg[0], vm->registers.count_erg);
    vm->registers.status_reg.fields.FLAG_FIELD.ma_flag = _b >= 0;
    vm->registers.status_reg.fields.FLAG_FIELD.nm_flag = _b < 0;
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

inline xVoid vm_execute_jump_directly(struct XVM * vm, inst * inst) {
    vm->registers.inst_reg += inst->jump.offset;
}

inline xVoid vm_execute_jump_if_eq(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.eq_flag)
        vm->registers.inst_reg += inst->jump.offset;

}

inline xVoid vm_execute_jump_if_ne(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.ne_flag)
        vm->registers.inst_reg += inst->jump.offset;
}

inline xVoid vm_execute_jump_if_gt(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.gt_flag)
        vm->registers.inst_reg += inst->jump.offset;
}

inline xVoid vm_execute_jump_if_lt(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.lt_flag)
        vm->registers.inst_reg += inst->jump.offset;
}

inline xVoid vm_execute_jump_if_ge(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.gt_flag
      ||vm->registers.status_reg.fields.FLAG_FIELD.eq_flag)
        vm->registers.inst_reg += inst->jump.offset;
}

inline xVoid vm_execute_jump_if_le(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.lt_flag
      ||vm->registers.status_reg.fields.FLAG_FIELD.eq_flag)
        vm->registers.inst_reg += inst->jump.offset;
}

inline xVoid vm_execute_jump_if_ma(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.ma_flag)
        vm->registers.inst_reg += inst->jump.offset;
}

inline xVoid vm_execute_jump_if_nm(struct XVM * vm, inst * inst) {
    if (vm->registers.status_reg.fields.FLAG_FIELD.nm_flag)
        vm->registers.inst_reg += inst->jump.offset;
}

inline xVoid vm_execute_call(struct XVM * vm, inst * inst) {
    mem_space * space = MemManager.get_space(vm->manager, RA_MEM_ID);
    MemSpace.push(space, &vm->registers.ret_addr_reg);
    vm->registers.ret_addr_reg = vm->registers.inst_reg + 1;
    vm->registers.call_level_reg = MemSpace.size(space);
    vm->registers.inst_reg += inst->jump.offset;
}

inline xVoid vm_execute_ret(struct XVM * vm, inst * inst) {
    vm->registers.inst_reg = vm->registers.ret_addr_reg;
    mem_space * space = MemManager.get_space(vm->manager, RA_MEM_ID);
    MemSpace.pop(space, &vm->registers.ret_addr_reg);
    vm->registers.call_level_reg = MemSpace.size(space);
}

#define write_reg(_rd, _value) do {*(_rd) = (_rd) ? (_value) : 0;} while (false)

inline xVoid vm_execute_load(struct XVM * vm, inst * inst) {
    xuLong * rd = vm_get_register(vm, inst->msl_reg.rd);
    xuLong * rs = vm_get_register(vm, inst->msl_reg.rs);
    rd = vm_virt2real(rd);
    rs = vm_virt2real(rs);
    write_reg(rd, ((xuLong *)rs)[inst->msl_reg.offset]);
}

inline xVoid vm_execute_load_imm(struct XVM * vm, inst * inst) {
    xuLong * rd = vm_get_register(vm, inst->load_imm.rd);
    write_reg(rd, inst->load_imm.imm);
}
