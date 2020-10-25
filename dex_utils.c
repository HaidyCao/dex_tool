//
// Created by Haidy on 2020/10/24.
//
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "dex_utils.h"
#include "dex_log.h"
#include "c_hex_utils.h"

#define ACCESS_FLAG_BUF_SIZE 128

#define ACC_PUBLIC                  0x1
#define ACC_PRIVATE                 0x2
#define ACC_PROTECTED               0x4
#define ACC_STATIC                  0x8
#define ACC_FINAL                   0x10
#define ACC_SYNCHRONIZED            0x20
#define ACC_VOLATILE                0x40
#define ACC_BRIDGE                  0x40
#define ACC_TRANSIENT               0x80
#define ACC_VARARGS                 0x80
#define ACC_NATIVE                  0x100
#define ACC_INTERFACE               0x200
#define ACC_ABSTRACT                0x400
#define ACC_STRICT                  0x800
#define ACC_SYNTHETIC               0x1000
#define ACC_ANNOTATION              0x2000
#define ACC_ENUM                    0x4000
#define ACC_CONSTRUCTOR             0x10000
#define ACC_DECLARED_SYNCHRONIZED   0x20000

#define ACC_FLAG_HAS(flag, acc) ((u4) flag & (u4) acc) != 0

#define L_ARRAY L"[]"

__thread static char g_access_flag_buf[ACCESS_FLAG_BUF_SIZE];

const char *dex_access_flag_to_string(u4 flag) {
    g_access_flag_buf[0] = '\0';

    if (ACC_FLAG_HAS(flag, ACC_PUBLIC)) {
        strcat(g_access_flag_buf, "public ");
    }

    if (ACC_FLAG_HAS(flag, ACC_PRIVATE)) {
        strcat(g_access_flag_buf, "private ");
    }

    if (ACC_FLAG_HAS(flag, ACC_PROTECTED)) {
        strcat(g_access_flag_buf, "protected ");
    }

    if (ACC_FLAG_HAS(flag, ACC_STATIC)) {
        strcat(g_access_flag_buf, "static ");
    }

    if (ACC_FLAG_HAS(flag, ACC_FINAL)) {
        strcat(g_access_flag_buf, "final ");
    }

    if (ACC_FLAG_HAS(flag, ACC_SYNCHRONIZED)) {
        strcat(g_access_flag_buf, "synchronized ");
    }

    if (ACC_FLAG_HAS(flag, ACC_NATIVE)) {
        strcat(g_access_flag_buf, "native ");
    }

    if (ACC_FLAG_HAS(flag, ACC_VOLATILE)) {
        strcat(g_access_flag_buf, "volatile ");
    }

    if (ACC_FLAG_HAS(flag, ACC_TRANSIENT)) {
        strcat(g_access_flag_buf, "transient ");
    }

    if (ACC_FLAG_HAS(flag, ACC_ABSTRACT)) {
        strcat(g_access_flag_buf, "abstract ");
    }

    if (ACC_FLAG_HAS(flag, ACC_INTERFACE)) {
        strcat(g_access_flag_buf, "interface ");
    }

    return g_access_flag_buf;
}

u4 dex_read_uleb128(const u1 **data) {
    const u1 *ptr = *data;
//    hex_dump((char *) ptr, 5, 0);

    u4 result = *(ptr++);
    if (result > 0x7f) {
        int cur = *(ptr++);
        result = (result & (u1) 0x7f) | (u4) (((u4) cur & (u1) 0x7f) << (u1) 7);
        if (cur > 0x7f) {
            cur = *(ptr++);
            result |= ((u4) cur & (u1) 0x7f) << (u1) 14;
            if (cur > 0x7f) {
                cur = *(ptr++);
                result |= ((u4) cur & (u1) 0x7f) << (u1) 21;
                if (cur > 0x7f) {
                    /*
                     * Note: We don't check to see if cur is out of
                     * range here, meaning we tolerate garbage in the
                     * high four-order bits.
                     */
                    cur = *(ptr++);
                    result |= (u4) cur << (u1) 28;
                }
            }
        }
    }
    *data = ptr;
    return result;
}

wchar_t *dex_L_type_to_class(wchar_t *type) {
    wchar_t *str = wcsdup(type + 1);
    ssize_t len = wcslen(str);
    for (ssize_t i = 0; i < len; ++i) {
        if (str[i] == L'/') {
            str[i] = L'.';
        }
    }

    str[len - 1] = L'\0';
    return str;
}

wchar_t *dex_type_to_java_class(wchar_t *type) {
    if (wcscmp(L"V", type) == 0) {
        return wcsdup(L"void");
    }

    if (wcscmp(L"Z", type) == 0) {
        return wcsdup(L"boolean");
    }

    if (wcscmp(L"B", type) == 0) {
        return wcsdup(L"byte");
    }

    if (wcscmp(L"S", type) == 0) {
        return wcsdup(L"short");
    }

    if (wcscmp(L"C", type) == 0) {
        return wcsdup(L"char");
    }

    if (wcscmp(L"I", type) == 0) {
        return wcsdup(L"int");
    }

    if (wcscmp(L"J", type) == 0) {
        return wcsdup(L"long");
    }

    if (wcscmp(L"F", type) == 0) {
        return wcsdup(L"float");
    }

    if (wcscmp(L"D", type) == 0) {
        return wcsdup(L"double");
    }

    if (wcsncmp(L"[", type, 1) == 0) {
        wchar_t *sub_str = dex_type_to_java_class(type + 1);
        if (sub_str == NULL) {
            return NULL;
        }

        wchar_t *new_str = realloc(sub_str, (wcslen(sub_str) + wcslen(L_ARRAY) + 1) * sizeof(wchar_t));
        if (new_str == NULL) {
            return NULL;
        }

        return wcscat(new_str, L_ARRAY);
    }

    if (wcsncmp(L"L", type, 1) == 0) {
        return dex_L_type_to_class(type);
    }

    LOGW("unknown type: %ls", type);
    return NULL;
}