//
// Project Name: xParse
// Filename: symbols.h
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#ifndef XPARSE_SYMBOLS_H
#define XPARSE_SYMBOLS_H

#include "char_t.h"

typedef enum {
    begin_quantifier = 1,
    end_quantifier,
    begin_charset,
    end_charset,
    begin_group,
    end_group,
    begin_switch, begin_refer = begin_switch,
    end_switch, end_refer = end_switch,
    begin_of_match,
    only_parse,
    call_label,
    call_value,
    inverse_match,
    end_of_match, charset_intersect = end_of_match,
    branch,
    closure,
    posure,
    lessure,
    range_to,
    set_label,
    colon,
    escape,
    comma,
    semicolon,
    ctrl_symbols,
    whitespace,
} __XPARSE_regexp_symbol_enum__; // NOLINT(*-reserved-identifier)

static const char_t * XPARSE_REGEXP_SYMBOLS[32];

typedef enum {
    begin_block = 1,
    end_block,
    begin_subscript,
    end_subscript,
    begin_parentheses,
    end_parentheses,
    lt,
    gt,
    bit_inv,
    bit_and,
    bit_or,
    bit_xor,
    bool_inv,
    bool_and,
    bool_or,
    mul,
    div,
    mod,
    add,
    sub,
    assign,
    eq,
    ge,
    le,
    eos,
    p_comma,
    str_quote,
    attr_access,
} __XPARSE_program_symbol_enum__; // NOLINT(*-reserved-identifier)

static const char_t * XPARSE_PROGRAM_SYMBOLS[32];


#define __eat_symbols(__sym, __MODE) do { \
    xuInt expect_len = strlen_o(XPARSE_##__MODE##_SYMBOLS[__sym]); \
    if (strcmp_o(XPARSE_##__MODE##_SYMBOLS[__sym], sp) != (expect_len)) { \
        parser->err_pos = sp; \
        parser->err_len = (expect_len); \
        return 0; \
    } \
    sp += (expect_len); \
} while (false)
#define regexp_sym_eat(__sym) __eat_symbols(__sym, REGEXP)
#define program_sym_eat(__sym) __eat_symbols(__sym, PROGRAM)

#define __equal_symbols(__sp, __sym, __MODE) (strcmp_o(XPARSE_##__MODE##_SYMBOLS[__sym], __sp) == strlen_o(XPARSE_##__MODE##_SYMBOLS[__sym]))
#define regexp_sym_eq(__sp, __sym) __equal_symbols(__sp, __sym, REGEXP)
#define program_sym_eq(__sp, __sym) __equal_symbols(__sp, __sym, PROGRAM)

#define __has_symbols(__sp, __sym, __MODE) (stridx_i(XPARSE_##__MODE##_SYMBOLS[__sym], *__sp, strlen_o(XPARSE_##__MODE##_SYMBOLS[__sym])) != -1)
#define regexp_sym_in(__sp, __sym) __has_symbols(__sp, __sym, REGEXP)
#define program_sym_in(__sp, __sym) __has_symbols(__sp, __sym, PROGRAM)


#endif //XPARSE_SYMBOLS_H
