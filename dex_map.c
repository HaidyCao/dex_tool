//
// Created by Haidy on 2020/10/25.
//
#include <stddef.h>

#include "dex_map.h"

#define TYPE_HEADER_ITEM                0x0000
#define TYPE_STRING_ID_ITEM             0x0001
#define TYPE_TYPE_ID_ITEM               0x0002
#define TYPE_PROTO_ID_ITEM              0x0003
#define TYPE_FIELD_ID_ITEM              0x0004
#define TYPE_METHOD_ID_ITEM             0x0005
#define TYPE_CLASS_DEF_ITEM             0x0006
#define TYPE_CALL_SITE_ID_ITEM          0x0007
#define TYPE_METHOD_HANDLE_ITEM         0x0008

#define TYPE_MAP_LIST                   0x1000
#define TYPE_TYPE_LIST                  0x1001
#define TYPE_ANNOTATION_SET_REF_LIST    0x1002
#define TYPE_ANNOTATION_SET_ITEM        0x1003

#define TYPE_CLASS_DATA_ITEM            0x2000
#define TYPE_CODE_ITEM                  0x2001
#define TYPE_STRING_DATA_ITEM           0x2002
#define TYPE_DEBUG_INFO_ITEM            0x2003
#define TYPE_ANNOTATION_ITEM            0x2004
#define TYPE_ENCODED_ARRAY_ITEM         0x2005
#define TYPE_ANNOTATIONS_DIRECTORY_ITEM 0x2006

#define TYPE_HIDDENAPI_CLASS_DATA_ITEM  0xF000

const DexMapList *dex_get_map_list(DexHeader *header) {
    if (header == NULL) {
        return NULL;
    }

    void *data = header;
    return data + header->map_off;
}