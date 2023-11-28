//
// Project Name: xParse
// Filename: parse_label_name.c
// Creator: Yaokai Liu
// Create Date: 2023-11-12
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "misc.h"
#include "xdef.h"
#include "string.h"
#include "symbols.h"
#include "structs.h"

xSize parse_eval_refer(xParser * parser, const char_t * const regexp, Capture ** const _capture) {
    const char_t * sp = regexp;

    trace_enter();

    regexp_sym_eat(begin_refer);

    xLong val; xInt len;
    if ((len = str2l_ud(sp, &val)) != 0) {
        check(val >= MemSpace.size(parser->GROUP_SPACE), len);
        *_capture = MemSpace.real_addr(parser->GROUP_SPACE, val);
        *_capture = ((Refer *) *_capture)->capture;
    } else {
        struct ReferName name = {.start = sp, .length = 0};
        while (*sp && !regexp_sym_eq(sp, end_refer)) {
            if (char_t('a') <= *sp && *sp <= char_t('z')) {
                name.length ++;
            } else if (char_t('a') <= *sp && *sp <= char_t('z')) {
                name.length ++;
            } else if (char_t('0') <= *sp && *sp <= char_t('9')) {
                name.length ++;
            } else {
                break;
            }
        }
        check(name.length != 0, 1);
        Refer * refer = nullptr;
        for (xSize i = 0; i < MemSpace.size(parser->LABEL_SPACE); i++) {
            Refer * cur_refer = MemSpace.real_addr(parser->LABEL_SPACE, i);
            if (cur_refer->name.length == name.length && strcmp_i(cur_refer->name.start, name.start, name.length)) {
                refer = cur_refer;
                break;
            }
        }
        check(refer, name.length);
        *_capture = refer->capture;
    }
    regexp_sym_eat(end_refer);

    return sp - regexp;
}


xSize parse_call_refer(xParser * parser, const char_t * const regexp, xVoid ** const _function) {
    const char_t * sp = regexp;

    trace_enter();

    regexp_sym_eat(begin_refer);

    xLong val; xInt len;
    if ((len = str2l_ud(sp, &val)) != 0) {
        check(val >= MemSpace.size(parser->GROUP_SPACE), len);
        *_function = MemSpace.real_addr(parser->GROUP_SPACE, val);
        *_function = ((Refer *) *_function)->function;
    } else {
        struct ReferName name = {.start = sp, .length = 0};
        while (*sp && !regexp_sym_eq(sp, end_refer)) {
            if (char_t('a') <= *sp && *sp <= char_t('z')) {
                name.length ++;
            } else if (char_t('a') <= *sp && *sp <= char_t('z')) {
                name.length ++;
            } else if (char_t('0') <= *sp && *sp <= char_t('9')) {
                name.length ++;
            } else {
                break;
            }
        }
        check(name.length != 0, 1);
        Refer * refer = nullptr;
        for (xSize i = 0; i < MemSpace.size(parser->LABEL_SPACE); i++) {
            Refer * cur_refer = MemSpace.real_addr(parser->LABEL_SPACE, i);
            if (cur_refer->name.length == name.length && strcmp_i(cur_refer->name.start, name.start, name.length)) {
                refer = cur_refer;
                break;
            }
        }
        check(refer, name.length);
        *_function = refer->function;
    }
    regexp_sym_eat(end_refer);

    return sp - regexp;
}
