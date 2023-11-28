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
#include "structs.h"

xSize parse_plains(xParser * parser, const char_t *regexp);
xSize parse_single_escape(xParser * parser, const char_t *regexp);
xSize parse_charset(xParser * parser, const char_t * regexp, mem_space * _return_inst_array);
xSize parse_eval_refer(xParser * parser, const char_t * regexp, Capture ** _capture);
xSize parse_call_refer(xParser * parser, const char_t * regexp, xVoid ** _function);

#endif //XPARSE_MISC_H
