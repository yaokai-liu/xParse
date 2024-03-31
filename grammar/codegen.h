//
// Project Name: xParse
// Filename: codegen.h
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#ifndef XPARSE_CODEGEN_H
#define XPARSE_CODEGEN_H

#include "char_t.h"
#include "xparse.h"

xInt codegen_sequence(xParser *parser, char_t *start, xSize n_plains);
xInt codegen_inverse(xParser * parser);
xInt codegen_begin(xParser * parser);

#endif //XPARSE_CODEGEN_H
