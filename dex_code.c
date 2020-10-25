//
// Created by Haidy on 2020/10/24.
//
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "dex_code.h"
#include "c_hex_utils.h"
#include "dex_bytecode.h"

#define CODE_HEADER_LENGTH 16

void dex_show_code(DexHeader *header, u4 offset, char *prefix) {
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

//    hex_dump(code->insns, sizeof(u2) * code->insns_size, 0);

    u4 parsed_size = 0;
    while (parsed_size < code->insns_size) {
        u4 code_size = 0;
        char *bytecode = dex_parse_code(header, &code_data, &code_size);

        printf("\n%s\t%s", dex_fix_prefix(prefix), bytecode);
        parsed_size += code_size;
        free(bytecode);
    }

//    exit(-1);
}