//
// Created by Haidy on 2020/10/25.
//

#ifndef DEX_TOOL_DEX_BYTECODE_H
#define DEX_TOOL_DEX_BYTECODE_H

#include "dex_defs.h"

char *dex_parse_code(DexHeader *header, const u1 **data, u4 *code_size);

#endif //DEX_TOOL_DEX_BYTECODE_H
