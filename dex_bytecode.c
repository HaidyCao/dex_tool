//
// Created by Haidy on 2020/10/25.
//
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "dex_bytecode.h"
#include "dex_log.h"
#include "dex_field.h"
#include "dex_method.h"
#include "dex_type.h"

#define FORMAT_NOT_FOUNT 0xFF

#define CODE_VALUE_TYPE_U_8 1
#define CODE_VALUE_TYPE_U_16 2
#define CODE_VALUE_TYPE_U_32 3
#define CODE_VALUE_TYPE_U_64 4

#define CODE_KIND_NUMBER    0
#define CODE_KIND_STRING    1
#define CODE_KIND_TYPE      2
#define CODE_KIND_FIELD     3
#define CODE_KIND_METH      4
#define CODE_KIND_SITE      5

const static char *g_code_kinds[] = {
        "string",
        "type",
        "field",
        "meth",
        "site",
        NULL
};

typedef struct {
    u1 type;

    void *ptr;
    u8 v;
    u1 kind;
} CodeValue;

typedef struct {
    u1 code;
    char *code_format;
    char *format_id;
    u1 format_index;
    char *syntax;
    u1 kinds[10];
} BytecodeOp;

typedef struct {
    char *id;
    char *format;
    u1 code_size;
    u1 registers_size;
} DalvikFormat;

#define DF_1 0
#define DF_2 5
#define DF_3 18
#define DF_4 29
#define DF_5 31

static DalvikFormat dalvik_formats[] = {
        {"10x",  "00|op"},
        {"12x",  "B|A|op"},
        {"11n",  "B|A|op"},
        {"11x",  "AA|op"},
        {"10t",  "AA|op"},

        // 5
        {"20t",  "00|op AAAA"},
        {"20bc", "AA|op BBBB"},
        {"22x",  "AA|op BBBB"},
        {"21t",  "AA|op BBBB"},
        {"21s",  "AA|op BBBB"},
        {"21h",  "AA|op BBBB"},
        {"21c",  "AA|op BBBB"},
        {"23x",  "AA|op CC|BB"},
        {"22b",  "AA|op CC|BB"},
        {"22t",  "B|A|op CCCC"},
        {"22s",  "B|A|op CCCC"},
        {"22c",  "B|A|op CCCC"},
        {"22cs", "B|A|op CCCC"},

        // 18
        {"30t",  "00|op AAAA AAAA"},
        {"32x",  "00|op AAAA BBBB"},
        {"31i",  "AA|op BBBB BBBB"},
        {"31t",  "AA|op BBBB BBBB"},
        {"31c",  "AA|op BBBB BBBB"},
        {"35c",  "A|G|op BBBB F|E|D|C"},
        {"35ms", "A|G|op BBBB F|E|D|C"},
        {"35mi", "A|G|op BBBB F|E|D|C"},
        {"3rc",  "AA|op BBBB CCCC"},
        {"3rms", "AA|op BBBB CCCC"},
        {"3rmi", "AA|op BBBB CCCC"},

        // 29
        {"45cc", "A|G|op BBBB F|E|D|C HHHH"},
        {"4rcc", "AA|op BBBB CCCC HHHH"},

        // 31
        {"51l",  "AA|op BBBB BBBB BBBB BBBB"},
};

static BytecodeOp bytecode_ops[] = {
        {0x00, "nop",                                             "10x",  FORMAT_NOT_FOUNT, "nop"},
        {0x01, "move v%d, v%d",                                   "12x",  FORMAT_NOT_FOUNT, "NULL"},
        {0x02, "move/from16 v%d, v%d",                            "22x",  FORMAT_NOT_FOUNT, NULL},
        {0x03, "move/16 v%d, v%d",                                "32x",  FORMAT_NOT_FOUNT, NULL},
        {0x04, "move-wide v%d, v%d",                              "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x05, "move-wide/from16 v%d, v%d",                       "22x",  FORMAT_NOT_FOUNT, NULL},
        {0x06, "move-wide/16 v%d, v%d",                           "32x",  FORMAT_NOT_FOUNT, NULL},
        {0x07, "move-object v%d, v%d",                            "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x08, "move-object/from16 v%d, v%d",                     "22x",  FORMAT_NOT_FOUNT, NULL},
        {0x09, "move-object/16 v%d, v%d",                         "32x",  FORMAT_NOT_FOUNT, NULL},
        {0x0a, "move-result v%d",                                 "11x",  FORMAT_NOT_FOUNT, NULL},
        {0x0b, "move-result-wide v%d",                            "11x",  FORMAT_NOT_FOUNT, NULL},
        {0x0c, "move-result-object v%d",                          "11x",  FORMAT_NOT_FOUNT, NULL},
        {0x0d, "move-exception v%d",                              "11x",  FORMAT_NOT_FOUNT, NULL},
        {0x0e, "return-void",                                     "10x",  FORMAT_NOT_FOUNT, NULL},
        {0x0f, "return v%d",                                      "11x",  FORMAT_NOT_FOUNT, NULL},
        {0x10, "return-wide v%d",                                 "11x",  FORMAT_NOT_FOUNT, NULL},
        {0x11, "return-object v%d",                               "11x",  FORMAT_NOT_FOUNT, NULL},
        {0x12, "const/4 v%d, %x",                                 "11n",  FORMAT_NOT_FOUNT, NULL},
        {0x13, "const/16 v%d, %x",                                "21s",  FORMAT_NOT_FOUNT, NULL},
        {0x14, "const v%d, %x",                                   "31i",  FORMAT_NOT_FOUNT, NULL},
        {0x15, "const/high16 v%d, %x",                            "21h",  FORMAT_NOT_FOUNT, NULL},
        {0x16, "const-wide/16 v%d, %x",                           "21s",  FORMAT_NOT_FOUNT, NULL},
        {0x17, "const-wide/32 v%d, %x",                           "31i",  FORMAT_NOT_FOUNT, NULL},
        {0x18, "const-wide v%d, %" PRId64,                        "51l",  FORMAT_NOT_FOUNT, NULL},
        {0x19, "const-wide/high16 v%d, %" PRId64,                 "21h",  FORMAT_NOT_FOUNT, NULL},
        {0x1a, "const-string v%d, \"%ls\"",                           "21c",  FORMAT_NOT_FOUNT, "const-string vAA, string@BBBB"},
        {0x1b, "const-string/jumbo v%d, \"%ls\"",                 "31c",  FORMAT_NOT_FOUNT, "const-string/jumbo vAA, string@BBBBBBBB"},
        {0x1c, "const-class v%d, %ls",                            "21c",  FORMAT_NOT_FOUNT, "const-class vAA, type@BBBB"},
        {0x1d, "monitor-enter v%d",                               "11x",  FORMAT_NOT_FOUNT, NULL},
        {0x1e, "monitor-exit v%d",                                "11x",  FORMAT_NOT_FOUNT, NULL},
        {0x1f, "check-cast v%d, %ls",                             "21c",  FORMAT_NOT_FOUNT, "check-cast vAA, type@BBBB"},
        {0x20, "instance-of v%d, v%d, %ls",                       "22c",  FORMAT_NOT_FOUNT, "instance-of vA, vB, type@CCCC"},
        {0x21, "array-length v%d, v%d",                           "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x22, "new-instance v%d, %ls",                           "21c",  FORMAT_NOT_FOUNT, "new-instance vAA, type@BBBB"},
        {0x23, "new-array v%d, v%d, %ls",                         "22c",  FORMAT_NOT_FOUNT, "new-array vA, vB, type@CCCC"},
        {0x24, "filled-new-array {%s}, %ls",                      "35c",  FORMAT_NOT_FOUNT, "filled-new-array/range {vCCCC .. vNNNN}, type@BBBB"},
        {0x25, "filled-new-array/range {v%d .. v%d}, %ls",        "3rc",  FORMAT_NOT_FOUNT, NULL},
        {0x26, "fill-array-data v%d, %" PRId64,                   "31t",  FORMAT_NOT_FOUNT, NULL},
        {0x27, "throw v%d",                                       "11x",  FORMAT_NOT_FOUNT, NULL},
        {0x28, "goto 0x%x",                                       "10t",  FORMAT_NOT_FOUNT, NULL},
        {0x29, "goto/16 0x%x",                                    "20t",  FORMAT_NOT_FOUNT, NULL},
        {0x2a, "goto/32 0x%x",                                    "30t",  FORMAT_NOT_FOUNT, NULL},
        {0x2b, "packed-switch v%d, 0x%x",                         "31t",  FORMAT_NOT_FOUNT, NULL},
        {0x2c, "sparse-switch v%d, 0x%x",                         "31t",  FORMAT_NOT_FOUNT, NULL},

        {0x2d, "cmpl-float v%d, v%d, v%d",                        "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x2e, "cmpg-float v%d, v%d, v%d",                        "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x2f, "cmpl-double v%d, v%d, v%d",                       "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x30, "cmpg-double v%d, v%d, v%d",                       "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x31, "cmp-long v%d, v%d, v%d",                          "23x",  FORMAT_NOT_FOUNT, NULL},

        {0x32, "if-eq v%d, v%d, 0x%x",                            "22t",  FORMAT_NOT_FOUNT, NULL},
        {0x33, "if-ne v%d, v%d, 0x%x",                            "22t",  FORMAT_NOT_FOUNT, NULL},
        {0x34, "if-lt v%d, v%d, 0x%x",                            "22t",  FORMAT_NOT_FOUNT, NULL},
        {0x35, "if-ge v%d, v%d, 0x%x",                            "22t",  FORMAT_NOT_FOUNT, NULL},
        {0x36, "if-gt v%d, v%d, 0x%x",                            "22t",  FORMAT_NOT_FOUNT, NULL},
        {0x37, "if-le v%d, v%d, 0x%x",                            "22t",  FORMAT_NOT_FOUNT, NULL},

        {0x38, "if-eqz v%d, 0x%x",                                "21t",  FORMAT_NOT_FOUNT, NULL},
        {0x39, "if-nez v%d, 0x%x",                                "21t",  FORMAT_NOT_FOUNT, NULL},
        {0x3a, "if-ltz v%d, 0x%x",                                "21t",  FORMAT_NOT_FOUNT, NULL},
        {0x3b, "if-gez v%d, 0x%x",                                "21t",  FORMAT_NOT_FOUNT, NULL},
        {0x3c, "if-gtz v%d, 0x%x",                                "21t",  FORMAT_NOT_FOUNT, NULL},
        {0x3d, "if-lez v%d, 0x%x",                                "21t",  FORMAT_NOT_FOUNT, NULL},

        {0x3e, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0x3f, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0x40, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0x41, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0x42, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0x43, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},

        {0x44, "aget v%d, v%d, v%d",                              "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x45, "aget-wide v%d, v%d, v%d",                         "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x46, "aget-object v%d, v%d, v%d",                       "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x47, "aget-boolean v%d, v%d, v%d",                      "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x48, "aget-byte v%d, v%d, v%d",                         "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x49, "aget-char v%d, v%d, v%d",                         "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x4a, "aget-short v%d, v%d, v%d",                        "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x4b, "aput v%d, v%d, v%d",                              "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x4c, "aput-wide v%d, v%d, v%d",                         "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x4d, "aput-object v%d, v%d, v%d",                       "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x4e, "aput-boolean v%d, v%d, v%d",                      "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x4f, "aput-byte v%d, v%d, v%d",                         "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x50, "aput-char v%d, v%d, v%d",                         "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x51, "aput-short v%d, v%d, v%d",                        "23x",  FORMAT_NOT_FOUNT, NULL},

        {0x52, "iget v%d, v%d, %ls",                              "22c",  FORMAT_NOT_FOUNT, "iinstanceop vA, vB, field@CCCC"},
        {0x53, "iget-wide v%d, v%d, %ls",                         "22c",  FORMAT_NOT_FOUNT, "iinstanceop vA, vB, field@CCCC"},
        {0x54, "iget-object v%d, v%d, %ls",                       "22c",  FORMAT_NOT_FOUNT, "iinstanceop vA, vB, field@CCCC"},
        {0x55, "iget-boolean v%d, v%d, %ls",                      "22c",  FORMAT_NOT_FOUNT, "iinstanceop vA, vB, field@CCCC"},
        {0x56, "iget-byte v%d, v%d, %ls",                         "22c",  FORMAT_NOT_FOUNT, "iinstanceop vA, vB, field@CCCC"},
        {0x57, "iget-char v%d, v%d, %ls",                         "22c",  FORMAT_NOT_FOUNT, "iinstanceop vA, vB, field@CCCC"},
        {0x58, "iget-short v%d, v%d, %ls",                        "22c",  FORMAT_NOT_FOUNT, "iinstanceop vA, vB, field@CCCC"},
        {0x59, "iput v%d, v%d, %ls",                              "22c",  FORMAT_NOT_FOUNT, "iinstanceop vA, vB, field@CCCC"},
        {0x5a, "iput-wide v%d, v%d, %ls",                         "22c",  FORMAT_NOT_FOUNT, "iinstanceop vA, vB, field@CCCC"},
        {0x5b, "iput-object v%d, v%d, %ls",                       "22c",  FORMAT_NOT_FOUNT, "iinstanceop vA, vB, field@CCCC"},
        {0x5c, "iput-boolean v%d, v%d, %ls",                      "22c",  FORMAT_NOT_FOUNT, "iinstanceop vA, vB, field@CCCC"},
        {0x5d, "iput-byte v%d, v%d, %ls",                         "22c",  FORMAT_NOT_FOUNT, "iinstanceop vA, vB, field@CCCC"},
        {0x5e, "iput-char v%d, v%d, %ls",                         "22c",  FORMAT_NOT_FOUNT, "iinstanceop vA, vB, field@CCCC"},
        {0x5f, "iput-short v%d, v%d, %ls",                        "22c",  FORMAT_NOT_FOUNT, "iinstanceop vA, vB, field@CCCC"},

        {0x60, "sget v%d, %ls",                                   "21c",  FORMAT_NOT_FOUNT, "sstaticop vAA, field@BBBB"},
        {0x61, "sget-wide v%d, %ls",                              "21c",  FORMAT_NOT_FOUNT, "sstaticop vAA, field@BBBB"},
        {0x62, "sget-object v%d, %ls",                            "21c",  FORMAT_NOT_FOUNT, "sstaticop vAA, field@BBBB"},
        {0x63, "sget-boolean v%d, %ls",                           "21c",  FORMAT_NOT_FOUNT, "sstaticop vAA, field@BBBB"},
        {0x64, "sget-byte v%d, %ls",                              "21c",  FORMAT_NOT_FOUNT, "sstaticop vAA, field@BBBB"},
        {0x65, "sget-char v%d, %ls",                              "21c",  FORMAT_NOT_FOUNT, "sstaticop vAA, field@BBBB"},
        {0x66, "sget-short v%d, %ls",                             "21c",  FORMAT_NOT_FOUNT, "sstaticop vAA, field@BBBB"},
        {0x67, "sput v%d, %ls",                                   "21c",  FORMAT_NOT_FOUNT, "sstaticop vAA, field@BBBB"},
        {0x68, "sput-wide v%d, %ls",                              "21c",  FORMAT_NOT_FOUNT, "sstaticop vAA, field@BBBB"},
        {0x69, "sput-object v%d, %ls",                            "21c",  FORMAT_NOT_FOUNT, "sstaticop vAA, field@BBBB"},
        {0x6a, "sput-boolean v%d, %ls",                           "21c",  FORMAT_NOT_FOUNT, "sstaticop vAA, field@BBBB"},
        {0x6b, "sput-byte v%d, %ls",                              "21c",  FORMAT_NOT_FOUNT, "sstaticop vAA, field@BBBB"},
        {0x6c, "sput-char v%d, %ls",                              "21c",  FORMAT_NOT_FOUNT, "sstaticop vAA, field@BBBB"},
        {0x6d, "sput-short v%d, %ls",                             "21c",  FORMAT_NOT_FOUNT, "sstaticop vAA, field@BBBB"},

        {0x6e, "invoke-virtual {%s}, %ls",                        "35c",  FORMAT_NOT_FOUNT, "invoke-kind {vC, vD, vE, vF, vG}, meth@BBBB"},
        {0x6f, "invoke-super {%s}, %ls",                          "35c",  FORMAT_NOT_FOUNT, "invoke-kind {vC, vD, vE, vF, vG}, meth@BBBB"},
        {0x70, "invoke-direct {%s}, %ls",                         "35c",  FORMAT_NOT_FOUNT, "invoke-kind {vC, vD, vE, vF, vG}, meth@BBBB"},
        {0x71, "invoke-static {%s}, %ls",                         "35c",  FORMAT_NOT_FOUNT, "invoke-kind {vC, vD, vE, vF, vG}, meth@BBBB"},
        {0x72, "invoke-interface {%s}, %ls",                      "35c",  FORMAT_NOT_FOUNT, "invoke-kind {vC, vD, vE, vF, vG}, meth@BBBB"},

        {0x73, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},

        {0x74, "invoke-interface/range {vd .. vd}, %ls",          "3rc",  FORMAT_NOT_FOUNT, "invoke-kind/range {vCCCC .. vNNNN}, meth@BBBB"},
        {0x75, "invoke-super/range {vd .. vd}, %ls",              "3rc",  FORMAT_NOT_FOUNT, "invoke-kind/range {vCCCC .. vNNNN}, meth@BBBB"},
        {0x76, "invoke-direct/range {vd .. vd}, %ls",             "3rc",  FORMAT_NOT_FOUNT, "invoke-kind/range {vCCCC .. vNNNN}, meth@BBBB"},
        {0x77, "invoke-static/range {vd .. vd}, %ls",             "3rc",  FORMAT_NOT_FOUNT, "invoke-kind/range {vCCCC .. vNNNN}, meth@BBBB"},
        {0x78, "invoke-interface/range {vd .. vd}, %ls",          "3rc",  FORMAT_NOT_FOUNT, "invoke-kind/range {vCCCC .. vNNNN}, meth@BBBB"},

        {0x79, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0x7a, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},

        {0x7b, "neg-int v%d, v%d",                                "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x7c, "not-int v%d, v%d",                                "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x7d, "neg-long v%d, v%d",                               "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x7e, "not-long v%d, v%d",                               "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x7f, "neg-float v%d, v%d",                              "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x80, "neg-double v%d, v%d",                             "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x81, "int-to-long v%d, v%d",                            "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x82, "int-to-float v%d, v%d",                           "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x83, "int-to-double v%d, v%d",                          "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x84, "long-to-int v%d, v%d",                            "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x85, "long-to-float v%d, v%d",                          "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x86, "long-to-double v%d, v%d",                         "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x87, "float-to-int v%d, v%d",                           "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x88, "float-to-long v%d, v%d",                          "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x89, "float-to-double v%d, v%d",                        "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x8a, "double-to-int v%d, v%d",                          "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x8b, "double-to-long v%d, v%d",                         "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x8c, "double-to-float v%d, v%d",                        "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x8d, "int-to-byte v%d, v%d",                            "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x8e, "int-to-char v%d, v%d",                            "12x",  FORMAT_NOT_FOUNT, NULL},
        {0x8f, "int-to-short v%d, v%d",                           "12x",  FORMAT_NOT_FOUNT, NULL},

        {0x90, "add-int v%d, v%d, v%d",                           "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x91, "sub-int v%d, v%d, v%d",                           "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x92, "mul-int v%d, v%d, v%d",                           "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x93, "div-int v%d, v%d, v%d",                           "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x94, "rem-int v%d, v%d, v%d",                           "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x95, "and-int v%d, v%d, v%d",                           "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x96, "or-int v%d, v%d, v%d",                            "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x97, "xor-int v%d, v%d, v%d",                           "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x98, "shl-int v%d, v%d, v%d",                           "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x99, "shr-int v%d, v%d, v%d",                           "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x9a, "ushr-int v%d, v%d, v%d",                          "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x9b, "add-long v%d, v%d, v%d",                          "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x9c, "sub-long v%d, v%d, v%d",                          "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x9d, "mul-long v%d, v%d, v%d",                          "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x9e, "div-long v%d, v%d, v%d",                          "23x",  FORMAT_NOT_FOUNT, NULL},
        {0x9f, "rem-long v%d, v%d, v%d",                          "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xa0, "and-long v%d, v%d, v%d",                          "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xa1, "or-long v%d, v%d, v%d",                           "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xa2, "xor-long v%d, v%d, v%d",                          "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xa3, "shl-long v%d, v%d, v%d",                          "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xa4, "shr-long v%d, v%d, v%d",                          "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xa5, "ushr-long v%d, v%d, v%d",                         "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xa6, "add-float v%d, v%d, v%d",                         "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xa7, "sub-float v%d, v%d, v%d",                         "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xa8, "mul-float v%d, v%d, v%d",                         "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xa9, "div-float v%d, v%d, v%d",                         "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xaa, "rem-float v%d, v%d, v%d",                         "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xab, "add-double v%d, v%d, v%d",                        "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xac, "sub-double v%d, v%d, v%d",                        "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xad, "mul-double v%d, v%d, v%d",                        "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xae, "div-double v%d, v%d, v%d",                        "23x",  FORMAT_NOT_FOUNT, NULL},
        {0xaf, "rem-double v%d, v%d, v%d",                        "23x",  FORMAT_NOT_FOUNT, NULL},

        {0xb0, "add-int/2addr v%d, v%d",                          "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xb1, "sub-int/2addr v%d, v%d",                          "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xb2, "mul-int/2addr v%d, v%d",                          "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xb3, "div-int/2addr v%d, v%d",                          "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xb4, "rem-int/2addr v%d, v%d",                          "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xb5, "and-int/2addr v%d, v%d",                          "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xb6, "or-int/2addr v%d, v%d",                           "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xb7, "xor-int/2addr v%d, v%d",                          "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xb8, "shl-int/2addr v%d, v%d",                          "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xb9, "shr-int/2addr v%d, v%d",                          "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xba, "ushr-int/2addr v%d, v%d",                         "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xbb, "add-long/2addr v%d, v%d",                         "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xbc, "sub-long/2addr v%d, v%d",                         "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xbd, "mul-long/2addr v%d, v%d",                         "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xbe, "div-long/2addr v%d, v%d",                         "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xbf, "rem-long/2addr v%d, v%d",                         "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xc0, "and-long/2addr v%d, v%d",                         "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xc1, "or-long/2addr v%d, v%d",                          "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xc2, "xor-long/2addr v%d, v%d",                         "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xc3, "shl-long/2addr v%d, v%d",                         "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xc4, "shr-long/2addr v%d, v%d",                         "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xc5, "ushr-long/2addr v%d, v%d",                        "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xc6, "add-float/2addr v%d, v%d",                        "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xc7, "sub-float/2addr v%d, v%d",                        "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xc8, "mul-float/2addr v%d, v%d",                        "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xc9, "div-float/2addr v%d, v%d",                        "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xca, "rem-float/2addr v%d, v%d",                        "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xcb, "add-double/2addr v%d, v%d",                       "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xcc, "sub-double/2addr v%d, v%d",                       "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xcd, "mul-double/2addr v%d, v%d",                       "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xce, "div-double/2addr v%d, v%d",                       "12x",  FORMAT_NOT_FOUNT, NULL},
        {0xcf, "rem-double/2addr v%d, v%d",                       "12x",  FORMAT_NOT_FOUNT, NULL},

        {0xd0, "add-int/lit16 v%d, v%d, 0x%x",                    "22s",  FORMAT_NOT_FOUNT, NULL},
        {0xd1, "rsub-int v%d, v%d, 0x%x",                         "22s",  FORMAT_NOT_FOUNT, NULL},
        {0xd2, "mul-int/lit16 v%d, v%d, 0x%x",                    "22s",  FORMAT_NOT_FOUNT, NULL},
        {0xd3, "div-int/lit16 v%d, v%d, 0x%x",                    "22s",  FORMAT_NOT_FOUNT, NULL},
        {0xd4, "rem-int/lit16 v%d, v%d, 0x%x",                    "22s",  FORMAT_NOT_FOUNT, NULL},
        {0xd5, "and-int/lit16 v%d, v%d, 0x%x",                    "22s",  FORMAT_NOT_FOUNT, NULL},
        {0xd6, "or-int/lit16 v%d, v%d, 0x%x",                     "22s",  FORMAT_NOT_FOUNT, NULL},
        {0xd7, "xor-int/lit16 v%d, v%d, 0x%x",                    "22s",  FORMAT_NOT_FOUNT, NULL},

        {0xd8, "add-int/lit8 v%d, v%d, 0x%x",                     "22b",  FORMAT_NOT_FOUNT, NULL},
        {0xd9, "rsub-int/lit8 v%d, v%d, 0x%x",                    "22b",  FORMAT_NOT_FOUNT, NULL},
        {0xda, "mul-int/lit8 v%d, v%d, 0x%x",                     "22b",  FORMAT_NOT_FOUNT, NULL},
        {0xdb, "div-int/lit8 v%d, v%d, 0x%x",                     "22b",  FORMAT_NOT_FOUNT, NULL},
        {0xdc, "rem-int/lit8 v%d, v%d, 0x%x",                     "22b",  FORMAT_NOT_FOUNT, NULL},
        {0xdd, "and-int/lit8 v%d, v%d, 0x%x",                     "22b",  FORMAT_NOT_FOUNT, NULL},
        {0xde, "or-int/lit8 v%d, v%d, 0x%x",                      "22b",  FORMAT_NOT_FOUNT, NULL},
        {0xdf, "xor-int/lit8 v%d, v%d, 0x%x",                     "22b",  FORMAT_NOT_FOUNT, NULL},
        {0xe0, "shl-int/lit8 v%d, v%d, 0x%x",                     "22b",  FORMAT_NOT_FOUNT, NULL},
        {0xe1, "shr-int/lit8 v%d, v%d, 0x%x",                     "22b",  FORMAT_NOT_FOUNT, NULL},
        {0xe2, "ushr-int/lit8 v%d, v%d, 0x%x",                    "22b",  FORMAT_NOT_FOUNT, NULL},

        {0xe3, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xe4, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xe5, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xe6, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xe7, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xe8, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xe9, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xea, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xeb, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xec, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xed, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xee, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xef, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xf0, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xf1, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xf2, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xf3, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xf4, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xf5, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xf6, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xf7, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xf8, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},
        {0xf9, "",                                                "10x",  FORMAT_NOT_FOUNT, NULL},

        {0xfa, "invoke-polymorphic {%s}, %ls, %ls",               "45cc", FORMAT_NOT_FOUNT, "invoke-polymorphic {vC, vD, vE, vF, vG}, meth@BBBB, proto@HHHH"},
        {0xfb, "invoke-polymorphic/range {v%d .. v%d}, %ls, %ls", "4rcc", FORMAT_NOT_FOUNT, "invoke-polymorphic/range {vCCCC .. vNNNN}, meth@BBBB, proto@HHHH"},
        {0xfc, "invoke-custom {%s}, %ls",                         "35c",  FORMAT_NOT_FOUNT, "invoke-custom {vC, vD, vE, vF, vG}, call_site@BBBB"},
        {0xfd, "invoke-custom/range {v%d .. v%d}, %ls",           "3rc",  FORMAT_NOT_FOUNT, "invoke-custom/range {vCCCC .. vNNNN}, call_site@BBBB"},
        {0xfe, "const-method-handle %v, %ls",                     "21c",  FORMAT_NOT_FOUNT, "const-method-handle vAA, method_handle@BBBB"},
        {0xff, "const-method-type %v, %ls",                       "21c",  FORMAT_NOT_FOUNT, "const-method-type vAA, proto@BBBB"},
};

__attribute__((constructor))
static void dex_bytecode_init() {
    size_t bytecode_ops_size = sizeof(bytecode_ops) / sizeof(BytecodeOp);
    size_t df_size = sizeof(dalvik_formats) / sizeof(DalvikFormat);

    LOGD("size of bytecode_ops = %lu", bytecode_ops_size);

    for (ssize_t i = 0; i < bytecode_ops_size; ++i) {
        BytecodeOp *op = &bytecode_ops[i];
        memset(op->kinds, 0xFF, sizeof(op->kinds));
        if (op->format_index == FORMAT_NOT_FOUNT) {
            ssize_t j;
            if (op->format_id[0] == '1') {
                j = DF_1;
            } else if (op->format_id[0] == '2') {
                j = DF_2;
            } else if (op->format_id[0] == '3') {
                j = DF_3;
            } else if (op->format_id[0] == '4') {
                j = DF_4;
            } else {
                j = DF_5;
            }

            for (j = 0; j < df_size; ++j) {
                if (strcmp(op->format_id, dalvik_formats[j].id) == 0) {
                    op->format_index = j;
                    break;
                }
            }
        }
    }
}

static bool save_args(CodeValue *args, u2 *code, u1 code_index, u1 bit_index, u1 pre_index, const u1 *args_item_size,
                      BytecodeOp *op) {
    CodeValue *arg = args + pre_index;
    if (args_item_size[pre_index] == 1 || args_item_size[pre_index] == 2) {
        u1 b = 0xFF;
        if (args_item_size[pre_index] == 1) {
            b = 0x0F;
        }
        u1 v = (u1) (code[code_index] >> (u1) (16 - bit_index)) & b;

        arg->v = v;
        arg->type = CODE_VALUE_TYPE_U_8;
        arg->kind = CODE_KIND_NUMBER;
        arg->ptr = NULL;
        return true;
    }

    u1 size = args_item_size[pre_index] / 2;
    if (size == 2) {
        u2 v = code[code_index];

        arg->v = v;
        arg->type = CODE_VALUE_TYPE_U_16;
        arg->kind = CODE_KIND_NUMBER;
        arg->ptr = NULL;

        char kink_str[8] = {'\0', '\0', '\0', '\0', '\0'};
        char ch = (char) ('A' + pre_index);
        sprintf(kink_str, "@%c%c%c%c", ch, ch, ch, ch);

        if (op->syntax != NULL) {
            arg->kind = op->kinds[pre_index];
            if (arg->kind == 0xFF) {
                char *result = strstr(op->syntax, kink_str);
                if (result != NULL) {
                    char all_kink_str[16];

                    const char **kinds = g_code_kinds;
                    int i = 0;
                    while (kinds[i] != NULL) {
                        bzero(all_kink_str, sizeof(all_kink_str));
                        sprintf(all_kink_str, "%s%s", kinds[i], kink_str);

                        if (strstr(op->syntax, all_kink_str) != NULL) {
                            arg->kind = op->kinds[pre_index] = i + 1;
                            break;
                        }
                        i++;
                    }
                }
            }
        }
        return true;
    } else if (size == 4) {
        u4 *v = (u4 *) (code + (code_index - 1));

        arg->v = *v;
        arg->type = CODE_VALUE_TYPE_U_32;
        arg->kind = CODE_KIND_NUMBER;
        arg->ptr = NULL;
        return true;
    } else if (size == 8) {
        u8 *v = (u8 *) (code + (code_index - 3));

        arg->v = *v;
        arg->type = CODE_VALUE_TYPE_U_64;
        arg->kind = CODE_KIND_NUMBER;
        arg->ptr = NULL;
        return true;
    }
    return false;
}

static void
handle_format_args(DexHeader *header, CodeValue *args, uint64_t *format_args, u1 format_arg_index, u1 arg_index) {
    switch (args[arg_index].kind) {
        case CODE_KIND_NUMBER:
            format_args[format_arg_index] = args[arg_index].v;
            return;
        case CODE_KIND_STRING:
            args[arg_index].ptr = dex_string_copy_and_release_old(dex_get_string_by_index(header, args[arg_index].v));
            break;
        case CODE_KIND_TYPE:
            args[arg_index].ptr = dex_string_copy_and_release_old(
                    dex_get_type_desc_by_index(header, args[arg_index].v));
            break;
        case CODE_KIND_FIELD:
            args[arg_index].ptr = dex_get_field_name_for_bytecode(header, args[arg_index].v);
            break;
        case CODE_KIND_METH:
            args[arg_index].ptr = dex_get_method_for_bytecode(header, args[arg_index].v);
            break;
        case CODE_KIND_SITE:

            break;
        default:
            break;
    }

    format_args[format_arg_index] = (uint64_t) args[arg_index].ptr;
}

static void handle_print_args(DexHeader *header, CodeValue *args, uint64_t *format_args, BytecodeOp *op) {
    u1 arg_index = 0;
    u1 last_index = 0xFF;
    ssize_t syntax_len = strlen(op->syntax);
    for (ssize_t i = 0; i < syntax_len; ++i) {
        if (op->syntax[i] >= 'A' && op->syntax[i] <= 'Z') {
            u1 index = op->syntax[i] - 'A';
            if (index == last_index) {
                continue;
            }
            last_index = index;
            handle_format_args(header, args, format_args, arg_index, index);

            arg_index++;
        }
    }
}

static char *parse_code(DexHeader *header, DalvikFormat *format, const u1 **data, BytecodeOp *op) {
    const u1 *ptr = *data;
    char *id = format->id;

    u1 reg_size;
    if (id[1] == 'r') {
        reg_size = 0xFF;
    } else {
        reg_size = id[1] - '0';

        // 35.. 45..
        if (reg_size == 5) {
            reg_size++;
        }
    }

    int args_size = reg_size;
    if (strlen(id) > 2) {
        for (int i = 2; i < strlen(id); ++i) {
            if (id[i] == 'b' || id[i] == 'h' || id[i] == 'i' || id[i] == 'l' || id[i] == 'n' || id[i] == 's' ||
                id[i] == 'c' || id[i] == 'm' || id[i] == 't') {
                args_size++;
            }
        }
    }

    CodeValue args[args_size];
    bzero(args, args_size);

    u1 args_item_size[args_size];
    bzero(args_item_size, args_size);
    u2 *code = (u2 *) ptr;

    u1 code_index = 0;
    u1 bit_index = 0;
    u1 pre_index = 0xFF;
    size_t format_len = strlen(format->format);
    for (int i = 0; i < format_len; ++i) {
        if (format->format[i] == '0' || format->format[i] == 'o' || format->format[i] == 'p') {
            bit_index += 4;
            continue;
        } else if (format->format[i] == '|') {
            if (pre_index == 0xFF) {
                continue;
            }

            if (save_args(args, code, code_index, bit_index, pre_index, args_item_size, op)) {
                pre_index = 0xFF;
            }
        } else if (format->format[i] == ' ') {
            if (pre_index == 0xFF) {
                code_index++;
                bit_index = 0;
                continue;
            }

            if (save_args(args, code, code_index, bit_index, pre_index, args_item_size, op)) {
                pre_index = 0xFF;
            }

            u1 next_index = format->format[i] - 'A';
            if (next_index != pre_index) {
                save_args(args, code, code_index, bit_index, pre_index, args_item_size, op);
            }
            code_index++;
            bit_index = 0;
        } else {
            int index = format->format[i] - 'A';
            if (index >= args_size) {
                LOGE("bad format: %s", format->format);
                return NULL;
            }

            args_item_size[index]++;
            pre_index = index;
            bit_index += 4;
        }

        if (i == format_len - 1) {
            save_args(args, code, code_index, bit_index, pre_index, args_item_size, op);
        }
    }

    u1 format_arg_count = args_size;
    uint64_t format_args[10];
    if (op->format_id[1] == '5') {
        format_arg_count -= 5;

        char format_tmp[16];
        bzero(format_tmp, sizeof(format_tmp));

        for (int i = 0; i < args[0].v; ++i) {
            if (i == 0) {
                sprintf(format_tmp, "v%d", (u1) args[2 + i].v);
            } else {
                sprintf(format_tmp, "%s, v%d", format_tmp, (u1) args[2 + i].v);
            }
        }

        args[0].ptr = strdup(format_tmp);
        format_args[0] = (uint64_t) args[0].ptr;
        handle_format_args(header, args, format_args, 1, 1); // BBBB
        // check HHHH
        if (format_arg_count > 2) {
            for (int i = 0; i < format_arg_count - 2; ++i) {
                u1 format_arg_index = i + 2;
                u1 arg_index = 'H' - 'A' + i;
                handle_format_args(header, args, format_args, format_arg_index, arg_index);
            }
        }
    } else if (op->format_id[1] == 'r') {

    } else {
        for (int i = 0; i < args_size; ++i) {
            handle_format_args(header, args, format_args, i, i);
        }
    }

    char *bytecode = NULL;
    if (format_arg_count == 0) {
        bytecode = strdup(op->code_format);
    } else if (format_arg_count == 1) {
        asprintf(&bytecode, op->code_format, format_args[0]);
    } else if (format_arg_count == 2) {
        asprintf(&bytecode, op->code_format, format_args[0], format_args[1]);
    } else if (format_arg_count == 3) {
        asprintf(&bytecode, op->code_format, format_args[0], format_args[1], format_args[2]);
    } else if (format_arg_count == 4) {
        asprintf(&bytecode, op->code_format, format_args[0], format_args[1], format_args[2], format_args[3]);
    } else if (format_arg_count == 5) {
        asprintf(&bytecode, op->code_format, format_args[0], format_args[1], format_args[2], format_args[3],
                 format_args[4]);
    } else if (format_arg_count == 6) {
        asprintf(&bytecode, op->code_format, format_args[0], format_args[1], format_args[2], format_args[3],
                 format_args[4],
                 format_args[5]);
    } else if (format_arg_count == 7) {
        asprintf(&bytecode, op->code_format, format_args[0], format_args[1], format_args[2], format_args[3],
                 format_args[4],
                 format_args[5], format_args[6]);
    } else if (format_arg_count == 8) {
        asprintf(&bytecode, op->code_format, format_args[0], format_args[1], format_args[2], format_args[3],
                 format_args[4],
                 format_args[5], format_args[6], format_args[7]);
    } else {

    }

    // free args
    for (int i = 0; i < args_size; ++i) {
        free(args[i].ptr);
    }

    return bytecode;
}

char *dex_parse_code(DexHeader *header, const u1 **data, u4 *code_size) {
    const u1 *ptr = *data;

    BytecodeOp *op = &bytecode_ops[*ptr];
    DalvikFormat *format = &dalvik_formats[op->format_index];
//    LOGT("id = %s\nformat = %s\ncode_format = %s", format->id, format->format, op->code_format);

    char *code = parse_code(header, format, &ptr, op);

    *code_size = op->format_id[0] - '0';
    ptr += (*code_size * 2);
    *data = ptr;
    return code;
}