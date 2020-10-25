//
// Created by Haidy on 2020/10/24.
//
#include <stddef.h>
#include <stdlib.h>

#include "dex_proto.h"
#include "dex_string.h"
#include "dex_type.h"
#include "dex_utils.h"

void dex_show_proto(DexHeader *header) {
    u4 p_index;

    for (p_index = 0; p_index < header->proto_ids.size; ++p_index) {
        printf("proto[%d]:\n", p_index);
        dex_show_proto_by_index(header, p_index, NULL);
    }
}

void dex_show_proto_by_index(DexHeader *header, u4 index, const char *prefix) {
    void *data = header;
    DexProtoId *ids = data + header->proto_ids.offset;
    if (index > header->proto_ids.size) {
        printf("%sproto: bad index: %d", dex_fix_prefix(prefix), index);
        return;
    }

    DexProtoId id = ids[index];

    wchar_t *shorty = dex_get_string_by_index(header, id.shorty_idx);
    printf("%s\tshorty\t\t: %ls;\n", dex_fix_prefix(prefix), shorty);
    dex_release_utf8(shorty);

    wchar_t *type_name = dex_get_type_desc_by_index(header, id.return_type_idx);
    wchar_t *java_name = dex_type_to_java_class(type_name);

    printf("%s\treturn type\t: %ls\n", dex_fix_prefix(prefix), java_name);
    dex_release_utf8(type_name);

    printf("%s\tparams\t\t: (", dex_fix_prefix(prefix));
    u4 p_offset = id.parameters_off;
    if (p_offset == 0) {
        printf(")\n");
        return;
    }

    DexTypeList *type_list = data + p_offset;
    for (u4 i = 0; i < type_list->size; ++i) {
//        wchar_t *str = dex_get_type_desc_by_index(header, type_list->list[i].type_idx);
//        printf("%ls", str);
//        dex_release_utf8(str);

        wchar_t *str = dex_get_type_desc_by_index(header, type_list->list[i].type_idx);
        dex_release_utf8(str);
        java_name = dex_type_to_java_class(str);

        if (i != 0) {
            printf(", ");
        }
        printf("%ls", java_name);

        free(java_name);
    }
    printf(")\n");
}

void dex_show_proto_to_java(DexHeader *header, u4 index, u4 access_flags, wchar_t *method, bool show_arg,
                            const char *prefix) {
    void *data = header;
    DexProtoId *ids = data + header->proto_ids.offset;
    if (index > header->proto_ids.size) {
        printf("%sproto: bad index: %d", dex_fix_prefix(prefix), index);
        return;
    }

    DexProtoId id = ids[index];

    printf("%s%s", dex_fix_prefix(prefix), dex_access_flag_to_string(access_flags));

    wchar_t *type_name = dex_get_type_desc_by_index(header, id.return_type_idx);
    wchar_t *java_name = dex_type_to_java_class(type_name);
    dex_release_utf8(type_name);
    printf("%ls ", java_name);
    free(java_name);

    printf("%ls(", method);

    if (id.parameters_off > 0) {
        DexTypeList *type_list = data + id.parameters_off;
        for (u4 i = 0; i < type_list->size; ++i) {
            wchar_t *str = dex_get_type_desc_by_index(header, type_list->list[i].type_idx);
            dex_release_utf8(str);
            java_name = dex_type_to_java_class(str);

            if (i != 0) {
                printf(", ");
            }
            printf("%ls", java_name);

            if (show_arg) {
                printf(" v%d", i);
            }

            free(java_name);
        }
    }

    printf(")");
}