//
// Created by Haidy on 2020/10/24.
//
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

#include "dex_string.h"
#include "dex_log.h"
#include "dex_utils.h"

#define DEFAULT_STRING_DECODE_SIZE 1024

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

wchar_t *dex_string_copy_and_release_old(wchar_t *str) {
    wchar_t *copy = wcsdup(str);
    dex_release_utf8(str);
    return copy;
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

wchar_t *dex_get_string_buffer() {
    return default_decode_buffer;
}

wchar_t *dex_string_resize(wchar_t *str, size_t old_len, size_t target, size_t *new_len) {
    size_t str_len = wcslen(str);
    size_t len = old_len;
    while (len < target) {
        size_t new_l = len + ((size_t) len >> (u1) 1);
        if (new_l < target) {
            new_l = target;
        }

        if (str == default_decode_buffer) {
            wchar_t *new_buf = malloc(new_l * sizeof(wchar_t));
            int j;
            for (j = 0; j < str_len; ++j) {
                new_buf[j] = str[j];
            }
            new_buf[j] = L'\0';
            len = new_l;
            str = new_buf;
        } else {
            wchar_t *new_buf = realloc(str, new_l * sizeof(wchar_t));
            if (new_buf == NULL) {
                return NULL;
            }
            len = new_l;
            str = new_buf;
        }
        *new_len = len;
    }

    return str;
}

DexWCharBuffer *dex_new_DexWCharBuffer(size_t size) {
    DexWCharBuffer *buffer = malloc(sizeof(DexWCharBuffer));
    buffer->size = size;
    buffer->buf = malloc(size * sizeof(wchar_t));

    return buffer;
}

void dex_DexWCharBuffer_init(DexWCharBuffer *buffer, size_t size) {
    buffer->size = size;
    buffer->buf = malloc(size * sizeof(wchar_t));
    buffer->buf[0] = L'\0';
}

bool dex_DexWCharBuffer_append(DexWCharBuffer *buffer, wchar_t *str) {
    size_t target = wcslen(buffer->buf) + wcslen(str);
    if (buffer->size <= target) {
        if (dex_string_resize(buffer->buf, buffer->size, target, &buffer->size) == NULL) {
            return false;
        }
    }

    wcscat(buffer->buf, str);
    return true;
}

wchar_t *dex_wstring_escape(wchar_t *str) {
    size_t len = wcslen(str);
    wchar_t escaped[len + 1];
    ssize_t e_i = 0;
    for (ssize_t i = 0; i < len; ++i) {
        wchar_t t = str[i];
        if (t == L'\a' || t == L'\b' || t == L'\f' || t == L'\n' || t == L'\t' || t == L'\v' || t == L'\\' ||
            t == L'\?' || t == L'\'' || t == L'\"') {
            escaped[e_i++] = L'\\';
            escaped[e_i++] = t;
        } else {
            escaped[e_i++] = t;
        }
    }
    escaped[e_i] = L'\0';

    return wcsdup(escaped);
}