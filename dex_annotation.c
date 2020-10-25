//
// Created by Haidy on 2020/10/24.
//
#include <stddef.h>
#include <string.h>

#include "dex_annotation.h"
#include "dex_field.h"
#include "dex_method.h"

void dex_show_annotation_by_offset(DexHeader *header, u4 offset, const char *prefix) {
    void *data = header;
    DexAnnotationsDirectory *ad = data + offset;

    char new_prefix[strlen(dex_fix_prefix(prefix)) + 2];
    sprintf(new_prefix, "%s\t", dex_fix_prefix(prefix));

    printf("%s\tfields              : [", dex_fix_prefix(prefix));
    if (ad->fields_size > 0) {
        printf("\n");
        for (int i = 0; i < ad->fields_size; ++i) {
            printf("%s", dex_fix_prefix(prefix));
            dex_show_field_by_index_to_java(header, ad->annotations[i].field_idx, 0, new_prefix);
        }
        printf("%s\t]\n", dex_fix_prefix(prefix));
    } else {
        printf("]\n");
    }

    printf("%s\tannotated methods   : [", dex_fix_prefix(prefix));

    if (ad->annotated_methods_size > 0) {
        for (int i = 0; i < ad->annotated_methods_size; ++i) {
            printf("%s\n", dex_fix_prefix(prefix));
            dex_show_method_by_index_to_java(header, ad->annotations[ad->fields_size + i].field_idx,
                                             0, false, new_prefix);
        }
        printf("\n%s\t]\n", dex_fix_prefix(prefix));
    } else {
        printf("]\n");
    }

    printf("%s\tannotated parameters: [", dex_fix_prefix(prefix));

    if (ad->annotated_parameters_size > 0) {
        for (int i = 0; i < ad->annotated_parameters_size; ++i) {
            printf("%s\n", dex_fix_prefix(prefix));
            dex_show_method_by_index_to_java(header,
                                             ad->annotations[ad->fields_size + ad->annotated_methods_size +
                                                             i].field_idx, 0, false, new_prefix);
        }
        printf("\n%s\t]\n", dex_fix_prefix(prefix));
    } else {
        printf("]\n");
    }
}