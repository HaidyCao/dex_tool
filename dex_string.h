//
// Created by Haidy on 2020/10/24.
//

#ifndef DEX_TOOL_DEX_STRING_H
#define DEX_TOOL_DEX_STRING_H

#include <wchar.h>

#include "dex_defs.h"

void dex_release_utf8(wchar_t *str);

void dex_show_strings(DexHeader *header);

wchar_t *dex_get_string_by_index(DexHeader *header, u4 index);

#endif //DEX_TOOL_DEX_STRING_H
