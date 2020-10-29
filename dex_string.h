//
// Created by Haidy on 2020/10/24.
//

#ifndef DEX_TOOL_DEX_STRING_H
#define DEX_TOOL_DEX_STRING_H

#include <wchar.h>

#include "dex_defs.h"

typedef struct {
    wchar_t *buf;
    size_t size;
} DexWCharBuffer;

void dex_release_utf8(wchar_t *str);

void dex_show_strings(DexHeader *header);

wchar_t *dex_get_string_by_index(DexHeader *header, u4 index);

wchar_t *dex_string_copy_and_release_old(wchar_t *str);

DexWCharBuffer *dex_new_DexWCharBuffer(size_t size);

void dex_DexWCharBuffer_init(DexWCharBuffer *buffer, size_t size);

bool dex_DexWCharBuffer_append(DexWCharBuffer *buffer, wchar_t *str);

wchar_t *dex_wstring_escape(wchar_t *str);

wchar_t *dex_string_resize(wchar_t *str, size_t old_len, size_t target, size_t *new_len);

#endif //DEX_TOOL_DEX_STRING_H
