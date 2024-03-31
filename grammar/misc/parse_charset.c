//
// Project Name: xParse
// Filename: parse_charset.c
// Creator: Yaokai Liu
// Create Date: 2023-10-27
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "xdef.h"
#include "symbols.h"
#include "misc.h"
#include "string.h"
#include "escape.h"
#include "inst.h"
#include "pseudo.h"

struct range {
    char_t begin;
    char_t end;
};

struct charset_data_array {
    mem_space * plains;
    mem_space * ranges;
};

struct charset_inst_array {
    mem_space * normal_inst_array;
    mem_space * inverse_inst_array;
};


#define ALLOC_REG       (parser->reg_alloc_count)

xSize parse_charset_object(xParser * parser, const char_t * regexp, mem_space * _return_inst_array);
xSize parse_charset_simple_object(xParser * parser, const char_t * regexp, mem_space * _return_inst_array);
xVoid codegen_charset_relocate_jump(mem_space * inst_array, xSize target_addr);
xInt codegen_charset_unit(mem_space * inst_array, mem_space * storage_array, struct charset_data_array * data_array, xParser * parser);

xSize parse_charset(xParser * parser, const char_t * const regexp, mem_space * _return_inst_array)  // NOLINT(*-no-recursion)
{
    const char_t * sp = regexp;
    trace_enter();

    struct charset_inst_array inst_array;
    inst_array.normal_inst_array  = MemSpace.new("normal inst", parser->allocator, sizeof(inst));
    inst_array.inverse_inst_array = MemSpace.new("inverse inst", parser->allocator, sizeof(inst));
    mem_space * return_inst_array = MemSpace.new("return inst", parser->allocator, sizeof(inst));

    MemSpace.init(inst_array.normal_inst_array);
    MemSpace.init(inst_array.inverse_inst_array);
    MemSpace.init(return_inst_array);

    enum: xBool {is_normal = false, is_inverse = true} ma_mode = is_normal;
    mem_space * inst_arrays[2] = {inst_array.normal_inst_array, inst_array.inverse_inst_array};
    mem_space * current_inst_array = inst_arrays[ma_mode];

    regexp_sym_eat(begin_charset);

    while (*sp && !regexp_sym_eq(sp, end_charset)) {
        if (regexp_sym_eq(sp, inverse_match)) {
            ma_mode = !ma_mode;
            current_inst_array = inst_arrays[ma_mode];
            sp ++;
        } else if (regexp_sym_in(sp, whitespace)) {
            while (regexp_sym_in(sp, whitespace)) sp++;
            continue;
        } else {
            sp += parse_charset_object(parser, sp, return_inst_array);
            if (parser->err_len > 0) { goto __parse_charset_failed; }
            MemSpace.concat_space(current_inst_array, return_inst_array);
            MemSpace_reinit(return_inst_array);
        }
    }

    regexp_sym_eat(end_charset);

    // TODO: concat normal and inverse inst array.

    codegen_charset_relocate_jump(inst_array.normal_inst_array, MemSpace.size(inst_array.normal_inst_array));
    codegen_charset_relocate_jump(inst_array.inverse_inst_array, MemSpace.size(inst_array.inverse_inst_array));


    MemSpace.del(return_inst_array);
    MemSpace.del(inst_array.normal_inst_array);
    MemSpace.del(inst_array.inverse_inst_array);
    trace_leave();
    return sp - regexp;

    __parse_charset_failed:
    MemSpace.del(return_inst_array);
    MemSpace.del(inst_array.normal_inst_array);
    MemSpace.del(inst_array.inverse_inst_array);
    return 0;
}

xVoid codegen_charset_relocate_jump(mem_space * inst_array, xSize target_addr) {
    xVoid * virt_inst = MemSpace.virt_last(inst_array);
    for (; virt_inst; virt_inst --) {
        inst * instruction = MemSpace.virt2real(inst_array, virt_inst);
        if (instruction->single.opcode == inst_jump_if_ma && instruction->jump.offset == 0) {
            xSize virt_addr = (target_addr - (MemSpace.virt_last(inst_array) - virt_inst) - 1);
            instruction->jump.offset =(xInt) (sizeof(inst) * virt_addr);
        }
    }
}


xSize parse_charset_object(xParser * parser, const char_t * const regexp, mem_space * return_inst_array) {
    const char_t * sp = regexp;
    trace_enter();

    if (regexp_sym_eq(sp, begin_charset)) {
        sp += parse_charset(parser, sp, return_inst_array);
        if (parser->err_len > 0) { return 0; }
        if (regexp_sym_eq(sp, charset_intersect)) {
            sp++;
            codegen_charset_relocate_jump(return_inst_array, MemSpace.size(return_inst_array));
            sp += parse_charset_object(parser, sp, return_inst_array);
            if (parser->err_len > 0) { return 0; }
        }
    } else if (regexp_sym_eq(sp, begin_refer)) {
        // TODO: parse refer
        if (regexp_sym_eq(sp, charset_intersect)) {
            sp++;
            codegen_charset_relocate_jump(return_inst_array, MemSpace.size(return_inst_array));
            sp += parse_charset_object(parser, sp, return_inst_array);
            if (parser->err_len > 0) { return 0; }
        }
    } else if (regexp_sym_eq(sp, escape)) {
        // TODO: parse escape
    } else {
        parse_charset_simple_object(parser, sp, return_inst_array);
    }

    MemSpace.del(return_inst_array);

    trace_leave();
    return sp - regexp;

}

xSize parse_charset_simple_object(xParser * parser, const char_t * const regexp, mem_space * inst_array) {
    const char_t * sp = regexp;
    trace_enter();

    mem_space * plains = MemSpace.new("Charset Normal Plains", parser->allocator, sizeof(char_t));
    mem_space * ranges = MemSpace.new("Charset Normal Ranges", parser->allocator, sizeof(struct range));
    MemSpace.init(plains);
    MemSpace.init(ranges);

    while (*sp) {
        if (regexp_sym_eq(sp, range_to)) {
            check(stridx_o(NON_PLAIN, *(sp - 1)) < 0, 1);
            sp++;
            check(stridx_o(NON_PLAIN, *sp) < 0, 1);
            char_t s = {};
            MemSpace.pop(plains, &s);
            struct range r = {.begin = s, .end = *sp};
            MemSpace.push(ranges, &r);
            sp++;
        } else if (regexp_sym_in(sp, ctrl_symbols)) {
            break;
        } else {
            char_t c = *sp;
            MemSpace.push(plains, &c);
            sp++;
        }
    }

    struct charset_data_array data_array = {.plains = plains, .ranges = ranges};
    codegen_charset_unit(inst_array, parser->CHAR_SPACE, &data_array, parser);

    MemSpace.del(plains);
    MemSpace.del(ranges);

    trace_leave();
    return sp - regexp;
}

#define MAX_UNSIGNED_SHORT ((1 << 16) - 1)

xInt codegen_charset_unit(mem_space * inst_array, mem_space * storage_array,
                          struct charset_data_array * data_array, xParser * parser) {
    char_t * virt_start; xInt inst_count = 0;
    xSize n_plains = MemSpace.size(data_array->plains);
    if (n_plains) {
        virt_start = MemSpace.virt_last(storage_array);
        MemSpace.concat_space(storage_array, data_array->plains);
        inst instruction;
        instruction.match_reg.opcode = inst_set_reg;
        instruction.match_reg.reg = ALLOC_REG;
        if (n_plains > MAX_UNSIGNED_SHORT) {
            inst_count += pseudo_load_imm64(n_plains, vm_count_reg, inst_array);
        } else { instruction.match_reg.offset = n_plains; }
        inst_count += pseudo_load_imm64((xuLong) virt_start, ALLOC_REG, inst_array);
        MemSpace.push(inst_array, &instruction);
        ALLOC_REG ++; inst_count ++;
        // offset is zero means to relocate
        inst_count += pseudo_jump(inst_jump_if_ma, 0, 0, inst_array);
    }
    xSize n_ranges = MemSpace.size(data_array->ranges);
    if (MemSpace.size(data_array->ranges)) {
        virt_start = MemSpace.virt_last(storage_array);
        virt_start = ((xuLong) virt_start & 1) ? virt_start + 1 : virt_start;
        MemSpace.concat_space(storage_array, data_array->ranges);
        inst instruction;
        instruction.match_reg.opcode = inst_range_reg;
        instruction.match_reg.reg = ALLOC_REG;
        if (n_ranges > MAX_UNSIGNED_SHORT) {
            inst_count += pseudo_load_imm64(n_ranges, vm_count_reg, inst_array);
        } else { instruction.match_reg.offset = n_ranges; }
        pseudo_load_imm64((xuLong) virt_start, ALLOC_REG, inst_array);
        MemSpace.push(inst_array, &instruction);
        ALLOC_REG ++; inst_count ++;
        // offset is zero means to relocated
        pseudo_jump(inst_jump_if_ma, 0, 0, inst_array);
    }
    return inst_count;
}

