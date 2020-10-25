//
// Created by Haidy on 2020/10/23.
//
#include <stdio.h>

#include "dex_header.h"

void dex_show_header(DexHeader *header) {
    printf("magic             : %c%c%c %c%c%c\n"
           "checksum          : %x\n"
           "signature         : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n"
           "file size         : %d\n"
           "header size       : %d\n"
           "endian tag        : %d\n"
           "link size         : %d\n"
           "link off          : %d\n"
           "map size          : %d\n"
           "string ids size   : %d\n"
           "string ids off    : %d\n"
           "type ids size     : %d\n"
           "type ids off      : %d\n"
           "proto ids size    : %d\n"
           "proto ids off     : %d\n"
           "field ids size    : %d\n"
           "field ids off     : %d\n"
           "method ids size   : %d\n"
           "method ids off    : %d\n"
           "class defs size   : %d\n"
           "class defs off    : %d\n"
           "data size         : %d\n"
           "data off          : %d\n"
           "\n",
           header->magic[0], header->magic[1], header->magic[2], header->magic[4], header->magic[5], header->magic[6],
           header->checksum,
           header->signature[0], header->signature[1], header->signature[2], header->signature[3],
           header->signature[4], header->signature[5], header->signature[6], header->signature[7],
           header->signature[8], header->signature[9], header->signature[10], header->signature[11],
           header->signature[12], header->signature[13], header->signature[14], header->signature[15],
           header->signature[16], header->signature[17], header->signature[18], header->signature[19],
           header->file_size,
           header->header_size,
           header->endian_tag,
           header->link.size,
           header->link.offset,
           header->map_off,
           header->string_ids.size,
           header->string_ids.offset,
           header->type_ids.size,
           header->type_ids.offset,
           header->proto_ids.size,
           header->proto_ids.offset,
           header->field_ids.size,
           header->field_ids.offset,
           header->method_ids.size,
           header->method_ids.offset,
           header->class_defs.size,
           header->class_defs.offset,
           header->data.size,
           header->data.offset
    );
}