//
// Created by Haidy on 2020/10/24.
//
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

#include "dex_string.h"
#include "dex_log.h"
#include "dex_utils.h"

#define DEFAULT_STRING_DECODE_SIZE 128

__thread wchar_t default_decode_buffer[DEFAULT_STRING_DECODE_SIZE];

static u4 read_utf16(const u1 *data, ssize_t *index) {
    ssize_t data_index = *index;
    u1 ch = data[data_index++];
    wchar_t wch;

    if (ch != 0 && ch <= 0x7F) {
        wch = ch;
    } else if ((ch & (u1) 0xC0) == 0xC0) {
        wch = (u1) 0x1F & ch;
        wch = (u4) wch << (u1) 6;

        ch = data[data_index++];
        wch = (u4) wch | ((u1) 0x3f & (u4) ch);
    } else {
        wch = (u1) 0x0F & ch;
        wch = (u4) wch << (u1) 12;

        ch = data[data_index++];
        wch = (u4) wch | (((u4) 0x3f & (u4) ch) << (u1) 6);

        ch = data[data_index++];
        wch = (u4) wch | ((u4) 0x3f & (u4) ch);
    }
    *index = data_index;
    return wch;
}

static wchar_t *decode_string(const u1 *data) {
    wchar_t *decode = default_decode_buffer;
    ssize_t decode_size = DEFAULT_STRING_DECODE_SIZE;
    ssize_t data_index = 0;
    ssize_t decode_index = 0;

    // read string char size
    u4 need_decode_size = dex_read_uleb128(&data);
    if (need_decode_size == 0) {
        decode[0] = 0;
        return decode;
    }

    while (true) {
        wchar_t wch = read_utf16(data, &data_index);
        if (wch >= 0xD800 && wch <= 0xDBFF) {
            wchar_t wch2 = read_utf16(data, &data_index);

            wchar_t new_wch = ((u4) (wch - 0xD800) << (u1) 16) | (u4) (wch2 - 0xDC00);
            wch = new_wch + 0x10000;
        }

        decode[decode_index++] = wch;

        if (decode_index == decode_size) {
            ssize_t new_decode_size = decode_size + ((u4) decode_size >> (u1) 1);
            if (decode_size == DEFAULT_STRING_DECODE_SIZE) {
                wchar_t *new_decode = malloc(sizeof(wchar_t) * new_decode_size);
                if (new_decode == NULL) {
                    return NULL;
                }

                // copy old data
                for (int i = 0; i < decode_size; ++i) {
                    new_decode[i] = decode[i];
                }

                decode_size = new_decode_size;
                decode = new_decode;
            } else {
                wchar_t *new_decode = realloc(decode, sizeof(wchar_t) * new_decode_size);
                if (new_decode == NULL) {
                    return NULL;
                }

                decode_size = new_decode_size;
                decode = new_decode;
            }
        }

        if (decode_index == need_decode_size) {
            break;
        }
    }

    decode[decode_index] = 0;
    return decode;
}

void dex_release_utf8(wchar_t *str) {
    if (str != default_decode_buffer) {
        free(str);
    }
}

void dex_show_strings(DexHeader *header) {
    void *data = header;
    u4 string_index;

    DexStringId *ids = data + header->string_ids.offset;
    for (string_index = 0; string_index < header->string_ids.size; string_index++) {
        wchar_t *decode = decode_string(data + ids[string_index].offset);
        printf("string[%d]: offset = %d: value = %ls\n", string_index, ids[string_index].offset, decode);
        dex_release_utf8(decode);
    }
}

wchar_t *dex_get_string_by_index(DexHeader *header, u4 index) {
    u1 *data = (u1 *) header;
    DexStringId *id = (DexStringId *) (data + header->string_ids.offset + index * sizeof(DexStringId));
    return decode_string(data + id->offset);
}