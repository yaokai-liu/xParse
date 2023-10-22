//
// Project Name: xParse
// Filename: symbols.c
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "symbols.h"

static const char_t * XPARSE_REGEXP_SYMBOLS[32] = {
        [begin_quantifier]  = string_t("{"),
        [end_quantifier]    = string_t("}"),
        [begin_charset]     = string_t("["),
        [end_charset]       = string_t("]"),
        [begin_group]       = string_t("("),
        [end_group]         = string_t(")"),
        [begin_label]       = string_t("<"),
        [end_label]         = string_t(">"),
        [begin_of_match]    = string_t("~"),
        [only_parse]        = string_t("!"),
        [call_label]        = string_t("@"),
        [call_value]        = string_t("$"),
        [inverse_match]     = string_t("^"),
        [end_of_match]      = string_t("&"),
        [branch]            = string_t("|"),
        [closure]           = string_t("*"),
        [posure]            = string_t("+"),
        [lessure]           = string_t("?"),
        [range_to]          = string_t("-"),
        [set_label]         = string_t("="),
        [colon]             = string_t(":"),
        [escape]            = string_t("\\"),
        [comma]             = string_t(","),
        [semicolon]         = string_t(";"),
};

static const char_t * XPARSE_PROGRAM_SYMBOLS[32] = {
        [begin_block]       = string_t("{"),
        [end_block]         = string_t("}"),
        [begin_subscript]   = string_t("["),
        [end_subscript]     = string_t("]"),
        [begin_parentheses] = string_t("("),
        [end_parentheses]   = string_t(")"),
        [lt]                = string_t("<"),
        [gt]                = string_t(">"),
        [bit_inv]           = string_t("~"),
        [bit_and]           = string_t("&"),
        [bit_or]            = string_t("|"),
        [bit_xor]           = string_t("^"),
        [bool_inv]          = string_t("!"),
        [bool_and]          = string_t("&&"),
        [bool_or]           = string_t("||"),
        [mul]               = string_t("*"),
        [div]               = string_t("/"),
        [mod]               = string_t("%"),
        [add]               = string_t("+"),
        [sub]               = string_t("-"),
        [assign]            = string_t(":="),
        [eq]                = string_t("=="),
        [ge]                = string_t(">="),
        [le]                = string_t("<="),
        [eos]               = string_t(";"),
        [p_comma]           = string_t(","),
        [str_quote]         = string_t("\""),
        [attr_access]       = string_t("."),
};

