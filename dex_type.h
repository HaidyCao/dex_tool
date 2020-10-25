//
// Created by Haidy on 2020/10/24.
//

#ifndef DEX_TOOL_DEX_TYPE_H
#define DEX_TOOL_DEX_TYPE_H

#include <wchar.h>
#include "dex_defs.h"

void dex_show_type(DexHeader *header);

DexTypeId *dex_get_type_by_index(DexHeader *header, u4 index);

wchar_t *dex_get_type_desc_by_index(DexHeader *header, u4 index);

#endif //DEX_TOOL_DEX_TYPE_H
