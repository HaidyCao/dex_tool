//
// Created by Haidy on 2020/10/24.
//

#ifndef DEX_TOOL_DEX_UTILS_H
#define DEX_TOOL_DEX_UTILS_H

#include "dex_defs.h"

const char *dex_access_flag_to_string(u4 flag);

u4 dex_read_uleb128(const u1 **data);

wchar_t *dex_type_to_java_class(wchar_t *type);

#endif //DEX_TOOL_DEX_UTILS_H
