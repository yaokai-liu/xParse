//
// Project Name: xParse
// Filename: string.c
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "string.h"


xInt stridx(const char_t *_string, char_t _chr) {
    for (xInt i = 0; _string[i]; i++) {
        if (_string[i] == _chr) return i;
    }
    return -1;
}

xInt strcmp_o(const char_t *_str1, const char_t *_str2) {
    xInt i = 0;
    while (_str1[i] == _str2[i] && _str1[i]) i++;
    return i;
}

xBool strcmp_i(const char_t *_str1, const char_t *_str2, xuLong len) {
    xInt i = 0;
    while (_str1[i] == _str2[i] && i < len) i++;
    return i == len;
}

xInt str2l_ud(const char_t *_string, xLong *val_dest) {
    xInt i = 0;
#define CONDITION (char_t('0') <= _string[i] && _string[i] <= char_t('9'))
    for (; CONDITION; i++) {
        *val_dest *= 10;
        *val_dest += _string[i] - char_t('0');
    }
    return i;
#undef CONDITION
}

xInt l2str_d(xLong val, char_t *dest_string) {
    if (val == 0) {
        dest_string[0] = char_t('0');
        return 1;
    }
    if (val < 0) {
        dest_string[0] = char_t('-');
        dest_string++;
        val = -val;
    }
    xInt len = 0;
    for (; val > 0; len++) {
        dest_string[len] = (char_t) (val % 10) + char_t('0'); // NOLINT(*-narrowing-conversions)
        val /= 10;
    }
    for (xInt j = 0; j < len / 2; j++) {
        char_t temp = dest_string[j];
        dest_string[j] = dest_string[len - 1 - j];
        dest_string[len - 1 - j] = temp;
    }
    return len;
}

xInt strcpy_c(char_t * _dest, const char_t * _src) {
    xInt i = 0;
    for(; _src[i]; i++) {
        _dest[i] = _src[i];
    }
    return i;
}


#ifndef XPARSE_USING_STD_FORMAT
#include <stdarg.h>

xInt strfmt_dsc(char_t * _dest, const char_t * _format, ...) {
    char_t * sp = _dest;
    va_list ap = {};
    xInt var_count = 0;
    for (xInt i = 0; _format[i]; i++) {
        if (_format[i] == char_t('%'))
            var_count ++;
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
                    // using xInt instead of char_t because arguments are aligned with at least 4 bytes.
                    *sp = va_arg(ap, xInt);
                    sp ++;
                    break;
                }
                case char_t('s'): {
                    sp += strcpy(sp, va_arg(ap, char_t *));
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
#endif