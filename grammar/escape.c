//
// Project Name: xParse
// Filename: escape.c
// Creator: Yaokai Liu
// Create Date: 2023-10-23
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "escape.h"

#define CONTROL_SYM "()[]{}<>\\|@#$&~:;=+-*/?!^."
#define BLANK_SYM "\n\r\v\f\t "

const char_t * const NON_PLAIN = string_t(BLANK_SYM CONTROL_SYM "\0");
const char_t * const SINGLE_ESCAPE = string_t("nrtfv " CONTROL_SYM "\0");
const char_t * const CLASS_ESCAPE = string_t("sSwWdDcC" "\0");
