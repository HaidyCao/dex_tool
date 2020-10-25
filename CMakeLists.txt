cmake_minimum_required(VERSION 3.17)
project(dex_tool C)

set(CMAKE_C_STANDARD 99)

add_executable(
        dex_tool

        main.c
        dex_utils.c
        dex_header.c
        dex_log.c
        c_hex_utils.c
        dex_string.c
        dex_defs.c
        dex_type.c
        dex_proto.c
        dex_field.c
        dex_method.c
        dex_class.c
        dex_annotation.c
        dex_code.c
        dex_bytecode.c
)