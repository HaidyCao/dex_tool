//
// Created by Haidy on 2020/10/24.
//

#ifndef DEX_TOOL_DEX_LOG_H
#define DEX_TOOL_DEX_LOG_H

#include <stdio.h>
#include <libgen.h>

#define LOGT(fmt, ...) printf("Trace: [%s(%d):%s]: " fmt "\n", basename(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define LOGD(fmt, ...) printf("Debug: [%s(%d):%s]: " fmt "\n", basename(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define LOGI(fmt, ...) printf("Info: [%s(%d):%s]: " fmt "\n", basename(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define LOGW(fmt, ...) printf("Warning: [%s(%d):%s]: " fmt "\n", basename(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define LOGE(fmt, ...) printf("Error: [%s(%d):%s]: " fmt "\n", basename(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)

#endif //DEX_TOOL_DEX_LOG_H
