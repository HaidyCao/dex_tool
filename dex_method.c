//
// Created by Haidy on 2020/10/24.
//
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include "dex_method.h"
#include "dex_proto.h"
#include "dex_type.h"

void dex_show_method(DexHeader *header) {
    for (u4 i = 0; i < header->method_ids.size; ++i) {
        printf("method[%d]: \n", i);
        dex_show_method_by_index(header, i, true, NULL);
    }
}

void dex_show_method_by_index(DexHeader *header, u4 index, bool show_class, const char *prefix) {
    void *data = header;

    DexMethod *methods = data + header->method_ids.offset;
    DexMethod method = methods[index];

    if (show_class) {
        wchar_t *class_type = dex_get_type_desc_by_index(header, method.class_idx);
        printf("%s\tclass : %ls\n", dex_fix_prefix(prefix), class_type);
        dex_release_utf8(class_type);
    }

    char new_prefix[strlen(dex_fix_prefix(prefix)) + 2];
    sprintf(new_prefix, "%s\t", dex_fix_prefix(prefix));

    printf("%s\tproto : \n", dex_fix_prefix(prefix));
    dex_show_proto_by_index(header, method.proto_idx, new_prefix);

    wchar_t *name = dex_get_string_by_index(header, method.name_idx);
    printf("%s\tname : %ls\n", dex_fix_prefix(prefix), name);
    dex_release_utf8(name);
}

void dex_show_method_by_index_to_java(DexHeader *header, u4 index, u4 access_flags,
                                      bool show_arg,
                                      const char *prefix) {
    void *data = header;

    DexMethod *methods = data + header->method_ids.offset;
    DexMethod method = methods[index];

    char new_prefix[strlen(dex_fix_prefix(prefix)) + 2];
    sprintf(new_prefix, "%s\t", dex_fix_prefix(prefix));

    wchar_t *name = dex_get_string_by_index(header, method.name_idx);
    wchar_t *method_name = wcsdup(name);
    dex_release_utf8(name);

    dex_show_proto_to_java(header, method.proto_idx, access_flags, method_name, show_arg, new_prefix);
    free(method_name);
}