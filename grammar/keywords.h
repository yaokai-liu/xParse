//
// Project Name: xParse
// Filename: keywords.h
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#ifndef XPARSE_KEYWORDS_H
#define XPARSE_KEYWORDS_H

#include "char_t.h"

typedef enum {
    VOID = 1,
    TOKEN,
    STRUCT,
    UNION,
    CHAR,
    INT16,
    INT32,
    INT64,
    STRING,
    BOOL,
    WHILE,
    IF,
    ELSE,
    TRUE,
    FALSE,
} __XPARSE_keyword_enum__; // NOLINT(*-reserved-identifier)

static const char_t * XPARSE_KEYWORDS[16];

#endif //XPARSE_KEYWORDS_H
