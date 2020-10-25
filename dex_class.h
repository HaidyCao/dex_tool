//
// Created by Haidy on 2020/10/24.
//

#ifndef DEX_TOOL_DEX_CLASS_H
#define DEX_TOOL_DEX_CLASS_H

#include "dex_string.h"
#include "dex_defs.h"

void dex_show_class();

void dex_show_class_by_index(DexHeader *header, u4 index, const char *prefix, DexOptions *options);

wchar_t *dex_get_class_type_by_index(DexHeader *header, u2 index);

#endif //DEX_TOOL_DEX_CLASS_H
