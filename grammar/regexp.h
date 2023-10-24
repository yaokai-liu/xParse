//
// Project Name: xParse
// Filename: sequence.h
// Creator: Yaokai Liu
// Create Date: 2023-10-23
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#ifndef XPARSE_REGEXP_H
#define XPARSE_REGEXP_H

#include "char_t.h"
#include "xparse.h"

xSize compile_begin(xParser * parser, char_t * regexp);
xSize compile_inverse(xParser * parser, char_t * regexp);
xSize compile_sequence(xParser * parser, char_t * regexp);
xSize compile_charset(xParser * parser, char_t * regexp);


#endif //XPARSE_REGEXP_H
