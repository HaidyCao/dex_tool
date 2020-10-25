//
// Created by Haidy on 2020/10/24.
//
#include <stdlib.h>
#include <stddef.h>

#include "dex_code.h"
#include "c_hex_utils.h"

#define CODE_HEADER_LENGTH 12

void dex_show_code(DexHeader *header, u4 offset) {
    void *data = header;
    DexCode *code = data + offset;

    const u1 *code_data = data + offset + CODE_HEADER_LENGTH;
    if (code->tries_size != 0 && code->ins_size % 2 == 1) {
        code_data += 2;
    }

    DexTry *tries = NULL;
    if (code->tries_size > 0) {
        tries = (DexTry *) code_data;
    }

    hex_dump(code->insns, sizeof(u2) * code->insns_size, 0);
}