//
// Created by Haidy on 2020/10/25.
//
#include <stddef.h>

#include "dex_bytecode.h"

#define FORMAT_NO_FOUNT 0xFF
#define FORMAT_REGISTER_SIZE 0xFF

#define NOP                     0x00
#define MOVE                    0x01
#define MOVE_FROM16             0x02
#define MOVE_16                 0x03
#define MOVE_WIDE               0x04
#define MOVE_WIDE_FROM16        0x05
#define MOVE_WIDE_6             0x06
#define MOVE_OBJECT             0x07
#define MOVE_OBJECT_FROM16      0x08
#define MOVE_OBJECT_16          0x09
#define MOVE_RESULT             0x0a
#define MOVE_RESULT_WIDE        0x0b
#define MOVE_RESULT_OBJECT      0x0c
#define MOVE_EXCEPTION          0x0d
#define RETURN_VOID             0x0e

typedef struct {
    u1 code;
    char *op;
    char *format_id;
    u1 format_index;
} BytecodeOp;

typedef struct {
    char *id;
    char *format;
    u1 code_size;
    u1 registers_size;
} DalvikFormat;

const DalvikFormat dalvik_formats[] = {
        {"10x",  "ØØ|op"},
        {"12x",  "B|A|op"},
        {"11n",  "B|A|op"},
        {"11x",  "AA|op"},
        {"10t",  "AA|op"},
        {"20t",  "ØØ|op AAAA"},
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
        {"30t",  "ØØ|op AAAA AAAA"},
        {"32x",  "ØØ|op AAAA BBBB"},
        {"31i",  "AA|op BBBB BBBB"},
        {"31t",  "AA|op BBBB BBBB"},
        {"31c",  "AA|op BBBB BBBB"},
        {"35c",  "A|G|op BBBB F|E|D|C"},
        {"35ms", "A|G|op BBBB F|E|D|C"},
        {"35mi", "A|G|op BBBB F|E|D|C"},
        {"3rc",  "AA|op BBBB CCCC"},
        {"3rms", "AA|op BBBB CCCC"},
        {"3rmi", "AA|op BBBB CCCC"},
        {"45cc", "A|G|op BBBB F|E|D|C HHHH"},
        {"4rcc", "AA|op BBBB CCCC HHHH"},
        {"51l",  "AA|op BBBB BBBB BBBB BBBB"},
};

const BytecodeOp bytecode_ops[] = {
        {0x00, "nop",         "10x", FORMAT_NO_FOUNT},
        {0x01, "move vA, vB", "12x", FORMAT_NO_FOUNT},
};

const char *dex_op(u1 op) {
    if (op == NOP) return "nop";
    if (op == MOVE) return "move";
    if (op == MOVE_FROM16) return "move/from16";
    if (op == MOVE_16) return "move/16";
    if (op == MOVE_WIDE) return "move-wide";
    if (op == MOVE_WIDE_FROM16) return "move-wide/from16";
    if (op == MOVE_WIDE_6) return "move-wide/16";
    if (op == MOVE_OBJECT) return "move-object";
    if (op == MOVE_OBJECT_FROM16) return "move-object/from";
    if (op == MOVE_OBJECT_16) return "move-object/16";
    if (op == MOVE_RESULT) return "move-result";
    if (op == MOVE_RESULT_WIDE) return "move-result-wide";
    if (op == MOVE_RESULT_OBJECT) return "move-result-object";
    if (op == MOVE_EXCEPTION) return "move-exception";
    if (op == RETURN_VOID) return "return-void";

}