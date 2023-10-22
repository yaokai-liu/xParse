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

xInt stridx(const char_t *_string, char_t _chr);

xInt strcmp_o(const char_t *_str1, const char_t *_str2);

xBool strcmp_i(const char_t *_str1, const char_t *_str2, xuLong len);

// String to long, while digits all are unsigned decimal.
xInt str2l_ud(const char_t *_string, xLong *val_dest);

// long to string
xInt l2str_d(xLong val, char_t *dest_string);

// string copy by single char
xInt strcpy_c(char_t * _dest, const char_t * _src);

#ifndef XPARSE_USING_STD_FORMAT
// format | only support for decimal, string and char.
xInt strfmt_dsc(char_t * _dest, const char_t * _format, ...);
#endif

#endif //XPARSE_STRING_H
