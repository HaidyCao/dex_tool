//
// Created by Haidy on 2020/10/24.
//
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "dex_field.h"
#include "dex_type.h"
#include "dex_string.h"
#include "dex_utils.h"

void dex_show_field(DexHeader *header) {
    for (u4 i = 0; i < header->field_ids.size; ++i) {
        printf("field[%d]:\n", i);

        dex_show_field_by_index(header, i, true, NULL);
    }
}

void dex_show_field_by_index(DexHeader *header, u4 index, bool show_class, const char *prefix) {
    void *data = header;
    DexFieldId *ids = data + header->field_ids.offset;
    DexFieldId id = ids[index];

    if (show_class) {
        wchar_t *class_type = dex_get_type_desc_by_index(header, id.class_idx);
        printf("%s\tclass   : %ls\n", dex_fix_prefix(prefix), class_type);
        dex_release_utf8(class_type);
    }

    wchar_t *type_desc = dex_get_type_desc_by_index(header, id.type_idx);
    printf("%s\ttype    : %ls\n", dex_fix_prefix(prefix), type_desc);
    dex_release_utf8(type_desc);

    wchar_t *name = dex_get_string_by_index(header, id.name_idx);
    printf("%s\tname    : %ls\n", dex_fix_prefix(prefix), name);
    dex_release_utf8(name);
}

void dex_show_field_by_index_to_java(DexHeader *header, u4 index, u4 access_flags, const char *prefix) {
    void *data = header;
    DexFieldId *ids = data + header->field_ids.offset;
    DexFieldId id = ids[index];

    wchar_t *type_desc = dex_get_type_desc_by_index(header, id.type_idx);
    wchar_t *java_type = dex_type_to_java_class(type_desc);
    dex_release_utf8(type_desc);

    wchar_t *name = dex_get_string_by_index(header, id.name_idx);

    printf("%s\t%s%ls %ls;\n", dex_fix_prefix(prefix), dex_access_flag_to_string(access_flags), java_type, name);
    dex_release_utf8(name);
    free(java_type);
}

wchar_t *dex_get_field_name(DexHeader *header, u4 index) {
    void *data = header;
    DexFieldId *ids = data + header->field_ids.offset;
    DexFieldId id = ids[index];

    return dex_get_string_by_index(header, id.name_idx);
}

wchar_t *dex_get_field_name_for_bytecode(DexHeader *header, u4 index) {
    void *data = header;
    DexFieldId *ids = data + header->field_ids.offset;
    DexFieldId id = ids[index];

    DexWCharBuffer buffer;
    dex_DexWCharBuffer_init(&buffer, 1024);

    wchar_t *class_name = dex_get_type_desc_by_index(header, id.class_idx);
    dex_DexWCharBuffer_append(&buffer, class_name);
    dex_release_utf8(class_name);

    dex_DexWCharBuffer_append(&buffer, L"->");

    wchar_t *name = dex_get_string_by_index(header, id.name_idx);
    dex_DexWCharBuffer_append(&buffer, name);
    dex_release_utf8(name);

    dex_DexWCharBuffer_append(&buffer, L":");

    wchar_t *type_name = dex_get_type_desc_by_index(header, id.type_idx);
    dex_DexWCharBuffer_append(&buffer, type_name);
    dex_release_utf8(type_name);

    return buffer.buf;
}