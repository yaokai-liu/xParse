//
// Project Name: xParse
// Filename: string.h
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#ifndef XPARSE_STRING_H
#define XPARSE_STRING_H

#include "char_t.h"

xInt stridx_o(const char_t *_string, char_t _chr);

xInt stridx_i(const char_t *_string, char_t _chr, xuLong len);

xInt strlen_o(const char_t *_string);

xInt strcmp_o(const char_t *_str1, const char_t *_str2);

xBool strcmp_i(const char_t *_str1, const char_t *_str2, xuLong len);

// String to long, while digits all are unsigned decimal.
xInt str2l_ud(const char_t *_string, xLong *val_dest);

// long to string
xInt l2str_d(xLong val, char_t *dest_string);

// string copy by single char
xInt strcpy_c(char_t * _dest, const char_t * _src);

#endif //XPARSE_STRING_H
