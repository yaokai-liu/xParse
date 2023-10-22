//
// Project Name: xParse
// Filename: char_t.h
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#ifndef XPARSE_CHAR_T_H
#define XPARSE_CHAR_T_H

#include "xtypes.h"

typedef xByte ascii;

#ifdef XPARSE_CHAR_USING_CHAR
typedef char   char_t;
#define char_t(x)     x
#elif defined(XPARSE_CHAR_USING_WCHAR)
#ifdef __WCHAR_TYPE__
    typedef __WCHAR_TYPE__  char_t;
#else
    typedef xInt     char_t;
#endif
#define char_t(x)      L##x
#define string_t(x)    L##x
#elif defined(XPARSE_CHAR_USING_ASCII)
typedef ascii char_t;
#define char_t(x) x
#define string_t(x)    ((char_t *) x)
#elif defined(XPARSE_CHAR_USING_XASCII)
typedef xAscii char_t;
#define char_t(x)      x
#define string_t(x)    ((xAscii *) (x))
#elif defined(XPARSE_CHAR_USING_XCHAR)
    typedef xChar char_t;
#define char_t(x)      ((xChar) x)
#define string_t(x)    ((xChar *) x)
#else
#error "You should specify character type."
#endif

#ifndef XPARSE_CHAR_USING_XCHAR

xuInt xparse_ord(char_t _the_char);

char_t xparse_chr(xuInt _the_order);

#else
#define xparse_ord     ord
#define xparse_chr     chr
#endif


#endif //XPARSE_CHAR_T_H
