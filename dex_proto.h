//
// Created by Haidy on 2020/10/24.
//

#ifndef DEX_TOOL_DEX_PROTO_H
#define DEX_TOOL_DEX_PROTO_H

#include "dex_defs.h"

void dex_show_proto(DexHeader *header);

void dex_show_proto_by_index(DexHeader *header, u4 index, const char *prefix);

void dex_show_proto_to_java(DexHeader *header, u4 index, u4 access_flags, wchar_t *method, bool show_arg,
                            const char *prefix);

#endif //DEX_TOOL_DEX_PROTO_H
