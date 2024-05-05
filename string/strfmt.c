/**
 * Project Name: xParse
 * Module Name: string
 * Filename: strfmt.c
 * Creator: Yaokai Liu
 * Create Date: 2024-03-29
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "strfmt.h"
#include "string.h"
typedef __builtin_va_list   va_list;
#define va_start(v, ...)    __builtin_va_start(v, 0)
#define va_end(v)	        __builtin_va_end(v)
#define va_arg(v,l)	        __builtin_va_arg(v,l)

xInt strfmt_dsc(char_t * _dest, const char_t * _format, ...) {
    char_t * sp = _dest;
    va_list ap = {};
    xInt var_count = 0;
    for (xInt i = 0; _format[i]; i++) {
        if (_format[i] == char_t('%')) var_count ++;
    }
    va_start(ap, _format);
    for (xInt i = 0; _format[i]; i++) {
        if (_format[i] == char_t('%')) {
            switch (_format[i + 1]) {
                case char_t('d'): {
                    sp += l2str_d(va_arg(ap, xInt), sp);
                    break;
                }
                case char_t('c'): {
                    // using xInt instead of char_t because
                    // arguments are aligned with at least 4 bytes.
                    *sp = va_arg(ap, xInt);
                    sp ++;
                    break;
                }
                case char_t('s'): {
                    sp += strcpy_c(sp, va_arg(ap, char_t *));
                    break;
                }
            }
            i++;
            continue;
        }
        *sp = _format[i];
        sp++;
    }
    va_end(ap);
    return (xInt) (sp - _dest);
}
