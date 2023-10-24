//
// Project Name: xParse
// Filename: misc.h
// Creator: Yaokai Liu
// Create Date: 2023-10-23
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#ifndef XPARSE_MISC_H
#define XPARSE_MISC_H

#include "char_t.h"
#include "xparse.h"

xSize parse_plains(xParser * parser, char_t *regexp);
xSize parse_single_escape(xParser * parser, char_t *regexp);

#endif //XPARSE_MISC_H
