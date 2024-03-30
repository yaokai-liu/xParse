//
// Project Name: xParse
// Filename: string.c
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "string.h"


xInt stridx_o(const char_t *_string, char_t _chr) {
    for (xInt i = 0; _string[i]; i++) {
        if (_string[i] == _chr) return i;
    }
    return -1;
}

xInt stridx_i(const char_t *_string, char_t _chr, xuLong len) {
    for (xInt i = 0; _string[i] && i < len; i++) {
        if (_string[i] == _chr) return i;
    }
    return -1;
}

xInt strlen_o(const char_t *_string){
    xInt i = 0;
    while (_string[i]) i++;
    return i;
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
    char_t * _dest = dest_string;
    if (val == 0) {
        _dest[0] = char_t('0');
        return 1;
    }
    if (val < 0) {
        _dest[0] = char_t('-');
        _dest++; val = -val;
        return 1 + l2str_d(val, _dest);
    }
    for (; val > 0; _dest++) {
        *(_dest) = (char_t) (val % 10) + char_t('0'); // NOLINT(*-narrowing-conversions)
        val /= 10;
    }
    xInt len = (xInt) (_dest - dest_string);
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
