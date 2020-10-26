//
// Created by Haidy on 2020/10/24.
//
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <wchar.h>

#include "dex_defs.h"
#include "dex_log.h"

static DexArgs g_args;

__thread DexOptions g_options = {
        false,
        NULL,
        NULL,
        false
};

DexArgs *dex_get_args() {
    return &g_args;
}

void dex_init_options(DexOptions *options) {
    options->search = g_args.search_str != NULL || g_args.compiled != NULL;
    if (g_args.search_str) {
        wchar_t w_str[strlen(g_args.search_str) + 1];
        bzero(&w_str, sizeof(w_str));
        if (mbstowcs(w_str, g_args.search_str, strlen(g_args.search_str)) == -1) {
            LOGE("mbstowcs failed: %s", strerror(errno));
        } else {
            free(options->search_str);
            options->search_str = wcsdup(w_str);
        }
    }
    options->search_reg_compiled = g_args.compiled;
    options->show_code = g_args.show_code;
}

DexOptions *dex_get_options() {
    dex_init_options(&g_options);
    return &g_options;
}

const char *dex_fix_prefix(const char *prefix) {
    if (prefix == NULL) {
        return "";
    }

    return prefix;
}

void DexBytecodeContext_init(DexBytecodeContext *context, DexHeader *header, const void *code) {
    context->header = header;
    context->code = code;
    context->code_index = 0;
    context->array_size = 0;
    context->array = NULL;
}

#define DEX_BYTECODE_PSEUDO_ITEM_RESIZE_REMAINDER 4

static void DexBytecodePseudoItem_append(DexBytecodePseudoItem **array, const u4 *array_size, u4 offset, char *name) {
    DexBytecodePseudoItem *ptr = *array;
    if (ptr == NULL) {
        ptr = malloc(sizeof(DexBytecodePseudoItem) * DEX_BYTECODE_PSEUDO_ITEM_RESIZE_REMAINDER);
    } else if (*array_size % DEX_BYTECODE_PSEUDO_ITEM_RESIZE_REMAINDER == 0) {
        ptr = realloc(ptr, (*array_size + DEX_BYTECODE_PSEUDO_ITEM_RESIZE_REMAINDER) * sizeof(DexBytecodePseudoItem));
    }

    ptr[*array_size].name = name;
    ptr[*array_size].offset = offset;

    *array = ptr;
}

char *DexBytecodeContext_pseudo_append(DexBytecodeContext *context, u2 type, u4 offset) {
    char *name = NULL;
    switch (type) {
        case DEX_PSEUDO_OPCODE_FILL_ARRAY_DATA_PAYLOAD:
            asprintf(&name, "array_%d", context->array_size);
            DexBytecodePseudoItem_append(&context->array, &context->array_size, offset, name);
            context->array_size++;
            break;
        default:
            break;
    }

    return name;
}