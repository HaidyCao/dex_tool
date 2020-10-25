//
// Created by Haidy on 2020/10/24.
//

#ifndef DEX_TOOL_DEX_FIELD_H
#define DEX_TOOL_DEX_FIELD_H

#include <stdbool.h>

#include "dex_defs.h"

void dex_show_field(DexHeader *header);

void dex_show_field_by_index(DexHeader *header, u4 index, bool show_class, const char *prefix);

void dex_show_field_by_index_to_java(DexHeader *header, u4 index, u4 access_flags, const char *prefix);

#endif //DEX_TOOL_DEX_FIELD_H
