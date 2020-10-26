//
// Created by Haidy on 2020/10/24.
//

#ifndef DEX_TOOL_DEX_DEFS_H
#define DEX_TOOL_DEX_DEFS_H

#include <stdbool.h>
#include <regex.h>
#include <stdlib.h>
#include <inttypes.h>

#define u1 unsigned char
#define u2 unsigned short
#define u4 unsigned int
#define u8 uint64_t

typedef struct {
    u4 size;
    u4 offset;
} DexHeaderSizeAndOffset;

typedef struct {
    u1 magic[8];
    u4 checksum;
    u1 signature[20];
    u4 file_size;
    u4 header_size;
    u4 endian_tag;
    DexHeaderSizeAndOffset link;
    u4 map_off;
    DexHeaderSizeAndOffset string_ids;
    DexHeaderSizeAndOffset type_ids;
    DexHeaderSizeAndOffset proto_ids;
    DexHeaderSizeAndOffset field_ids;
    DexHeaderSizeAndOffset method_ids;
    DexHeaderSizeAndOffset class_defs;
    DexHeaderSizeAndOffset data;
} DexHeader;

typedef struct {
    DexHeader *header;
    bool show_header;
    bool show_string;
    bool show_all;
    bool show_type;
    bool show_proto;
    bool show_field;
    bool show_method;
    bool show_class;
    bool show_code;
    char *search_str;
    regex_t *compiled;
} DexArgs;

typedef struct {
    bool search;
    wchar_t *search_str;
    regex_t *search_reg_compiled;

    bool show_code;
} DexOptions;

typedef struct {
    u4 offset;
} DexStringId;

typedef struct {
    u4 descriptor_idx;
} DexTypeId;

typedef struct {
    u4 shorty_idx;
    u4 return_type_idx;
    u4 parameters_off;
} DexProtoId;

typedef struct {
    u2 type_idx;
} DexTypeItem;

typedef struct {
    u4 size;
    DexTypeItem list[];
} DexTypeList;

typedef struct {
    u2 class_idx;
    u2 type_idx;
    u4 name_idx;
} DexFieldId;

typedef struct {
    u4 class_idx;
    u4 access_flags;
    u4 superclass_idx;
    u4 interfaces_off;
    u4 source_file_idx;
    u4 annotations_off;
    u4 class_data_off;
    u4 static_values_off;
} DexClassDef;

typedef struct {
    u2 class_idx;
    u2 proto_idx;
    u4 name_idx;
} DexMethod;

typedef struct {
    u4 field_idx;
    u4 annotations_off;
} DexAnnotation;

typedef struct {
    u4 class_annotations_off;
    u4 fields_size;
    u4 annotated_methods_size;
    u4 annotated_parameters_size;
    DexAnnotation annotations[];
} DexAnnotationsDirectory;

typedef struct {
    u4 field_idx;
    u4 access_flags;
} DexEncodedField;

typedef struct {
    u4 method_idx;
    u4 access_flags;
    u4 code_off;
} DexEncodedMethod;

typedef struct {
    u4 static_fields_size;
    u4 instance_fields_size;
    u4 direct_methods_size;
    u4 virtual_methods_size;
    DexEncodedField *static_fields;
    DexEncodedField *instance_fields;
    DexEncodedMethod *direct_methods;
    DexEncodedMethod *virtual_methods;
} DexClassData;

typedef struct {
    u2 registers_size;
    u2 ins_size;
    u2 outs_size;
    u2 tries_size;
    u4 debug_info_off;
    u4 insns_size;
    u2 insns[];
} DexCode;

typedef struct {
    u4 start_addr;
    u2 insn_count;
    u2 handler_off;
} DexTry;

typedef struct {
    u4 type_idx;
    u4 addr;
} DexEncodedTypeAddrPair;

typedef struct {
    int size;
    DexEncodedTypeAddrPair *handlers;
    u4 catch_all_addr;
} DexEncodedCacheHandler;

typedef struct {
    u2 type;
    u2 _;
    u4 size;
    u4 offset;
} DexMap;

typedef struct {
    u4 size;
    DexMap list[];
} DexMapList;

typedef struct {
    u4 offset;
    char *name;
} DexBytecodePseudoItem;

typedef struct {
    DexHeader *header;
    const void *code;
    u4 code_index;

    u4 array_size;
    DexBytecodePseudoItem *array;
} DexBytecodeContext;

void DexBytecodeContext_init(DexBytecodeContext *context, DexHeader *header, const void *code);

char *DexBytecodeContext_pseudo_append(DexBytecodeContext *context, u2 type, u4 offset);

#define DEX_PSEUDO_OPCODE_FILL_ARRAY_DATA_PAYLOAD   0x0300

typedef struct {
    u2 ident;
    u2 element_width;
    u4 size;
    u1 data[];
} DexFillArrayDataPayload;

DexArgs *dex_get_args();

void dex_init_options(DexOptions *options);

DexOptions *dex_get_options();

const char *dex_fix_prefix(const char *prefix);

#endif //DEX_TOOL_DEX_DEFS_H
