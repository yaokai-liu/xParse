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
    begin_label, begin_switch = begin_label,
    end_label, end_switch = end_label,
    begin_of_match,
    only_parse,
    call_label,
    call_value,
    inverse_match,
    end_of_match,
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


#define eat_symbols(sym) do { \
    xuInt expect_len = strlen_o(XPARSE_PROGRAM_SYMBOLS[sym]); \
    if (strcmp_o(XPARSE_PROGRAM_SYMBOLS[sym], sp) != (expect_len)) { \
        parser->err_pos = sp; \
        parser->err_len = (expect_len); \
        return 0; \
    } \
    sp += (expect_len); \
} while (false)



#endif //XPARSE_SYMBOLS_H
