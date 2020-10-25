//
// Created by Haidy on 2020/10/24.
//

#ifndef DEX_TOOL_DEX_METHOD_H
#define DEX_TOOL_DEX_METHOD_H

#include <stdbool.h>

#include "dex_defs.h"
#include "dex_string.h"

void dex_show_method(DexHeader *header);

void dex_show_method_by_index(DexHeader *header, u4 index, bool show_class, const char *prefix);

void dex_show_method_by_index_to_java(DexHeader *header, u4 index, u4 access_flags,
                                      bool show_arg,
                                      const char *prefix);

wchar_t *dex_get_method_for_bytecode(DexHeader *header, u4 index);

#endif //DEX_TOOL_DEX_METHOD_H
