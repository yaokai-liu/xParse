//
// Project Name: xParse
// Filename: keywords.c
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "keywords.h"


const char_t * XPARSE_KEYWORDS[16] = {
        [VOID]      = string_t("void"),
        [TOKEN]     = string_t("token"),
        [STRUCT]    = string_t("struct"),
        [UNION]     = string_t("union"),
        [CHAR]      = string_t("char"),
        [INT16]     = string_t("int16"),
        [INT32]     = string_t("int32"),
        [INT64]     = string_t("int64"),
        [STRING]    = string_t("string"),
        [BOOL]      = string_t("bool"),
        [WHILE]     = string_t("while"),
        [IF]        = string_t("if"),
        [ELSE]      = string_t("else"),
        [TRUE]      = string_t("true"),
        [FALSE]     = string_t("false"),
};
