//
// Created by Haidy on 2020/10/24.
//
#include <wchar.h>
#include <string.h>

#include "dex_type.h"
#include "dex_string.h"
#include "c_hex_utils.h"

void dex_show_type(DexHeader *header) {
    void *data = header;
    u4 type_size = header->type_ids.size;
    u4 type_index;

    printf("----------------"
           " type count: %d "
           "----------------"
           "\n\n", type_size);

//    hex_dump(data + header->type_ids.offset, header->type_ids.size * sizeof(DexTypeId), header->type_ids.offset);
    DexTypeId *ids = data + header->type_ids.offset;
    for (type_index = 0; type_index < type_size; ++type_index) {
        wchar_t *type_name = dex_get_string_by_index(header, ids[type_index].descriptor_idx);

        printf("type[%d]: idx: %d value: %ls\n", type_index, ids[type_index].descriptor_idx, type_name);

        dex_release_utf8(type_name);
    }
}

DexTypeId *dex_get_type_by_index(DexHeader *header, u4 index) {
    void *data = header;
    DexTypeId *ids = data + header->type_ids.offset;
    return ids + index;
}

wchar_t *dex_get_type_desc_by_index(DexHeader *header, u4 index) {
    DexTypeId *id = dex_get_type_by_index(header, index);
    return dex_get_string_by_index(header, id->descriptor_idx);
}