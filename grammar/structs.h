//
// Project Name: xParse
// Filename: struct.h
// Creator: Yaokai Liu
// Create Date: 2023-11-12
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#ifndef XPARSE_STRUCTS_H
#define XPARSE_STRUCTS_H

#include "char_t.h"
#include "mem_space.h"

typedef struct {
    char_t *start;
    xSize   length;
} Capture;

typedef struct Refer {
    struct ReferName {
        const char_t *  start;
        xSize           length;
    }           name;           // if start is zero, length means group index
    Capture *   capture;        // virt addr from parser's capture stack
    xVoid *     function;      // begin of real inst code of refer
} Refer;

#endif //XPARSE_STRUCTS_H

