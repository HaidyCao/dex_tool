//
// Created by Haidy on 2020/10/24.
//
#include <stddef.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#include "dex_class.h"
#include "dex_type.h"
#include "dex_annotation.h"
#include "dex_utils.h"
#include "dex_field.h"
#include "dex_method.h"
#include "dex_code.h"

const u4 NO_INDEX = 0xffffffff;

void dex_show_class() {
    const DexArgs *args = dex_get_args();
    DexHeader *header = args->header;

    DexOptions *options = dex_get_options();
    for (u4 i = 0; i < header->class_defs.size; ++i) {
        if (!options->search) {
            printf("class[%d]: \n", i);
        }
        dex_show_class_by_index(header, i, NULL, options);
    }
}

DexClassData *dex_decode_class_data(DexHeader *header, u4 offset) {
    const u1 *data = (u1 *) header;
    data = data + offset;

    DexClassData *class_data = calloc(1, sizeof(DexClassData));

    class_data->static_fields_size = dex_read_uleb128(&data);
    class_data->instance_fields_size = dex_read_uleb128(&data);
    class_data->direct_methods_size = dex_read_uleb128(&data);
    class_data->virtual_methods_size = dex_read_uleb128(&data);

    if (class_data->static_fields_size > 0) {
        class_data->static_fields = malloc(class_data->static_fields_size * sizeof(DexEncodedField));
    }

    if (class_data->instance_fields_size > 0) {
        class_data->instance_fields = malloc(class_data->instance_fields_size * sizeof(DexEncodedField));
    }

    if (class_data->direct_methods_size > 0) {
        class_data->direct_methods = malloc(class_data->direct_methods_size * sizeof(DexEncodedMethod));
    }

    if (class_data->virtual_methods_size > 0) {
        class_data->virtual_methods = malloc(class_data->virtual_methods_size * sizeof(DexEncodedMethod));
    }

    u4 pre_idx;
    for (ssize_t i = 0; i < class_data->static_fields_size; ++i) {
        u4 diff = dex_read_uleb128(&data);
        if (i == 0) {
            pre_idx = diff;
            diff = 0;
        }

        pre_idx = class_data->static_fields[i].field_idx = pre_idx + diff;
        class_data->static_fields[i].access_flags = dex_read_uleb128(&data);
    }

    for (ssize_t i = 0; i < class_data->instance_fields_size; ++i) {
        u4 diff = dex_read_uleb128(&data);
        if (i == 0) {
            pre_idx = diff;
            diff = 0;
        }

        pre_idx = class_data->instance_fields[i].field_idx = pre_idx + diff;
        class_data->instance_fields[i].access_flags = dex_read_uleb128(&data);
    }

    for (ssize_t i = 0; i < class_data->direct_methods_size; ++i) {
        u4 diff = dex_read_uleb128(&data);
        if (i == 0) {
            pre_idx = diff;
            diff = 0;
        }

        pre_idx = class_data->direct_methods[i].method_idx = pre_idx + diff;
        class_data->direct_methods[i].access_flags = dex_read_uleb128(&data);
        class_data->direct_methods[i].code_off = dex_read_uleb128(&data);
    }

    for (ssize_t i = 0; i < class_data->virtual_methods_size; ++i) {
        u4 diff = dex_read_uleb128(&data);
        if (i == 0) {
            pre_idx = diff;
            diff = 0;
        }

        pre_idx = class_data->virtual_methods[i].method_idx = pre_idx + diff;
        class_data->virtual_methods[i].access_flags = dex_read_uleb128(&data);
        class_data->virtual_methods[i].code_off = dex_read_uleb128(&data);
    }

    return class_data;
}

void DexClassData_free(DexClassData *data) {
    free(data->static_fields);
    free(data->instance_fields);
    free(data->direct_methods);
    free(data->virtual_methods);

    free(data);
}

void dex_show_class_data(DexHeader *header, u4 offset, const char *prefix, DexOptions *options) {
    char new_prefix[strlen(dex_fix_prefix(prefix)) + 2];
    sprintf(new_prefix, "%s\t", dex_fix_prefix(prefix));

    DexClassData *class_data = dex_decode_class_data(header, offset);
    printf("%s\tstatic_fields:      [", dex_fix_prefix(prefix));
    if (class_data->static_fields_size > 0) {
        printf("\n");
        for (ssize_t i = 0; i < class_data->static_fields_size; ++i) {
            dex_show_field_by_index_to_java(header, class_data->static_fields[i].field_idx,
                                            class_data->static_fields[i].access_flags, new_prefix);
        }
        printf("%s\t]\n", dex_fix_prefix(prefix));
    } else {
        printf("]\n");
    }

    printf("%s\tinstance_fields:    [", dex_fix_prefix(prefix));
    if (class_data->instance_fields_size > 0) {
        printf("\n");
        for (ssize_t i = 0; i < class_data->instance_fields_size; ++i) {
            dex_show_field_by_index_to_java(header, class_data->instance_fields[i].field_idx,
                                            class_data->instance_fields[i].access_flags, new_prefix);
        }
        printf("%s\t]\n", dex_fix_prefix(prefix));
    } else {
        printf("]\n");
    }

    printf("%s\tdirect_methods:     [", dex_fix_prefix(prefix));
    if (class_data->direct_methods_size > 0) {
        printf("\n");
        for (ssize_t i = 0; i < class_data->direct_methods_size; ++i) {
            dex_show_method_by_index_to_java(header, class_data->direct_methods[i].method_idx,
                                             class_data->direct_methods[i].access_flags, false, new_prefix);
            u4 code_off = class_data->direct_methods[i].code_off;
            if (options != NULL && options->show_code && code_off != 0) {
                char code_prefix[strlen(dex_fix_prefix(new_prefix)) + 2];
                sprintf(code_prefix, "%s\t", dex_fix_prefix(new_prefix));

                dex_show_code(header, code_off, code_prefix);
            }
            printf("\n");
        }
        printf("%s\t]\n", dex_fix_prefix(prefix));
    } else {
        printf("]\n");
    }

    printf("%s\tvirtual_methods:[", dex_fix_prefix(prefix));
    if (class_data->virtual_methods_size > 0) {
        printf("\n");
        for (ssize_t i = 0; i < class_data->virtual_methods_size; ++i) {
            dex_show_method_by_index_to_java(header, class_data->virtual_methods[i].method_idx,
                                             class_data->virtual_methods[i].access_flags, false, new_prefix);

            printf("\n");
        }
        printf("%s\t]\n", dex_fix_prefix(prefix));
    } else {
        printf("]\n");
    }

    DexClassData_free(class_data);
}

void dex_show_class_by_index(DexHeader *header, u4 index, const char *prefix, DexOptions *options) {
    void *data = header;
    DexClassDef *defs = data + header->class_defs.offset;

    char new_prefix[strlen(dex_fix_prefix(prefix)) + 2];
    sprintf(new_prefix, "%s\t", dex_fix_prefix(prefix));

    DexClassDef def = defs[index];
    wchar_t *name = dex_get_type_desc_by_index(header, def.class_idx);
    wchar_t *java_name = dex_type_to_java_class(name);

    dex_release_utf8(name);
    if (options != NULL && options->search) {
        if (java_name == NULL) {
            return;
        }

        if (options->search_str != NULL) {
            if (wcsstr(java_name, options->search_str) == NULL) {
                free(java_name);
                return;
            }
        } else if (options->search_reg_compiled != NULL) {
            size_t java_name_len = wcslen(java_name);
            char m_java_name[java_name_len * 3 + 2];
            bzero(m_java_name, sizeof(m_java_name));
            wcstombs(m_java_name, java_name, java_name_len);
            if (regexec(options->search_reg_compiled, m_java_name, 0, NULL, 0) == REG_NOMATCH) {
                free(java_name);
                return;
            }
        }

        printf("%ssearch:\n", dex_fix_prefix(prefix));
    }

    printf("%s\tname            : %ls\n", dex_fix_prefix(prefix), java_name);
    free(java_name);

    printf("%s\taccess_flags    : %s\n", dex_fix_prefix(prefix), dex_access_flag_to_string(def.access_flags));

    if (def.superclass_idx == NO_INDEX) {
        printf("%s\tsuper class     : java.lang.Object\n", dex_fix_prefix(prefix));
    } else {
        wchar_t *super_class_name = dex_get_type_desc_by_index(header, def.superclass_idx);
        wchar_t *super_class_java_name = dex_type_to_java_class(super_class_name);
        dex_release_utf8(super_class_name);
        printf("%s\tsuper class     : %ls\n", dex_fix_prefix(prefix), super_class_java_name);
        free(super_class_java_name);
    }

    if (def.interfaces_off == 0) {
        printf("%s\tinterfaces      : []\n", dex_fix_prefix(prefix));
    } else {
        DexTypeList *type_list = data + def.interfaces_off;
        printf("%s\tinterfaces      : [\n", dex_fix_prefix(prefix));
        for (u4 i = 0; i < type_list->size; ++i) {
            wchar_t *i_name = dex_get_type_desc_by_index(header, type_list->list[i].type_idx);
            wchar_t *i_java_name = dex_type_to_java_class(i_name);
            dex_release_utf8(i_name);
            printf("%s\t\t%ls;\n", dex_fix_prefix(prefix), i_java_name);
            free(i_java_name);
        }
        printf("%s\t]\n", dex_fix_prefix(prefix));
    }

    if (def.source_file_idx == NO_INDEX) {
        printf("%s\tsource file     : NO\n", dex_fix_prefix(prefix));
    } else {
        wchar_t *file = dex_get_string_by_index(header, def.source_file_idx);
        printf("%s\tsource file     : %ls\n", dex_fix_prefix(prefix), file);
        dex_release_utf8(file);
    }

    if (def.annotations_off == 0) {
        printf("%s\tannotations     : NO\n", dex_fix_prefix(prefix));
    } else {
        printf("%s\tannotations     : {\n", dex_fix_prefix(prefix));
        dex_show_annotation_by_offset(header, def.annotations_off, new_prefix);
        printf("%s\t}\n", dex_fix_prefix(prefix));
    }

    if (def.class_data_off == 0) {
        printf("%s\tclass data      : NO\n", dex_fix_prefix(prefix));
    } else {
        printf("%s\tclass data      : {\n", dex_fix_prefix(prefix));
        dex_show_class_data(header, def.class_data_off, new_prefix, options);
        printf("%s\t}\n", dex_fix_prefix(prefix));
    }
}

wchar_t *dex_get_class_type_by_index(DexHeader *header, u2 index) {
    void *data = header;
    if (index >= header->class_defs.size) {
        return NULL;
    }

    DexClassDef *defs = data + header->class_defs.offset;
    return dex_get_type_desc_by_index(header, defs[index].class_idx);
}