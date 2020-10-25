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