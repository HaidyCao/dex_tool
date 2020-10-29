#include <stdio.h>
#include <getopt.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <stdlib.h>

#include "dex_log.h"
#include "c_hex_utils.h"
#include "dex_header.h"
#include "dex_string.h"
#include "dex_type.h"
#include "dex_proto.h"
#include "dex_field.h"
#include "dex_method.h"
#include "dex_class.h"

#define ARG_HELP    0
#define ARG_FIND    1
#define ARG_EFIND   2
#define ARG_CODE    3

static const struct option long_options[] = {
        {"header", no_argument,       NULL, 'h'},
        {"string", no_argument,       NULL, 's'},
        {"type",   no_argument,       NULL, 't'},
        {"proto",  no_argument,       NULL, 'p'},
        {"field",  no_argument,       NULL, 'f'},
        {"method", no_argument,       NULL, 'm'},
        {"class",  no_argument,       NULL, 'c'},
        {"find",   required_argument, NULL, ARG_FIND},
        {"efind",  required_argument, NULL, ARG_EFIND},
        {"code",   no_argument,       NULL, ARG_CODE},
        {"all",    no_argument,       NULL, 'a'},
        {"help",   no_argument,       NULL, ARG_HELP}
};

static void usage() {
    printf("dex_tool usage: \n"
           "--header -h     : show dex header info\n"
           "--string -s     : show dex string list\n"
           "--type -t       : show dex type list\n"
           "--proto -p      : show dex proto list\n"
           "--field -f      : show dex field list\n"
           "--method -m     : show dex method list\n"
           "--class -c      : show dex class list\n"
           "--find          : find class\n"
           "--efind         : find class by regex\n"
           "--code          : print code\n"
           "--all -a        : show all data\n"
           "--help          : show this help and exit\n"
           "\n"
           "dex_tool dex_path -h\n"
           "\n");
}

/**
 * params
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv) {
    if (argc == 1) {
        usage();
        return -1;
    }
    setlocale(LC_ALL, "zh_CN.UTF-8");

    char *dex_path = argv[1];
    LOGT("dex_path = %s", dex_path);

    DexArgs *args = dex_get_args();

    int opt;
    while ((opt = getopt_long(argc - 1, argv + 1, "d:hsatpfmc", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                args->show_header = true;
                break;
            case 's':
                args->show_string = true;
                break;
            case 'a':
                args->show_all = true;
                break;
            case 't':
                args->show_type = true;
                break;
            case 'p':
                args->show_proto = true;
                break;
            case 'f':
                args->show_field = true;
                break;
            case 'm':
                args->show_method = true;
                break;
            case 'c':
                args->show_class = true;
                break;
            case ARG_HELP:
                usage();
                return 0;
            case ARG_FIND:
                args->search_str = strdup(optarg);
                break;
            case ARG_EFIND:
                args->compiled = malloc(sizeof(regex_t));
                int reg_error_code = regcomp(args->compiled, optarg,
                                             (u4) REG_ICASE | (u4) REG_EXTENDED | (u4) REG_NEWLINE);
                if (reg_error_code != 0) {
                    char err_msg[1024];
                    bzero(err_msg, sizeof(err_msg));
                    regerror(reg_error_code, args->compiled, err_msg, sizeof(err_msg));
                    printf("regcomp failed: %s: %s\n", err_msg, optarg);
                    return -1;
                }
                break;
            case ARG_CODE:
                args->show_code = true;
                break;
            default:
                printf("unknown opt: %s\n", optarg);
                usage();
                return -1;
        }
    }

    if (dex_path == NULL) {
        usage();
        return -1;
    }

    LOGT("show dex header = %d", args->show_header);

    // check dex file
    struct stat dex_stat;
    int dex_stat_result = stat(dex_path, &dex_stat);
    if (dex_stat_result == -1) {
        LOGE("stat dex file failed: %s", strerror(errno));
        return -1;
    }
    LOGT("dex_stat_result = %d", dex_stat_result);

    int dex_fd = open(dex_path, O_RDONLY);
    if (dex_fd == -1) {
        LOGE("open dex file failed: %s", strerror(errno));
        return -1;
    }

    void *data = mmap(NULL, dex_stat.st_size, PROT_READ, MAP_PRIVATE, dex_fd, 0);
    if (data == NULL) {
        LOGE("map dex file failed: %s", strerror(errno));
        return -1;
    }

    hex_dump(data, sizeof(DexHeader), 0);

    DexHeader *dex_header = data;
    args->header = dex_header;

    if (args->show_header || args->show_all) {
        dex_show_header(dex_header);
    }

    if (args->show_string || args->show_all) {
        dex_show_strings(dex_header);
    }

    if (args->show_type || args->show_all) {
        dex_show_type(dex_header);
    }

    if (args->show_proto || args->show_all) {
        dex_show_proto(dex_header);
    }

    if (args->show_field || args->show_all) {
        dex_show_field(dex_header);
    }

    if (args->show_method || args->show_all) {
        dex_show_method(dex_header);
    }

    if (args->show_class || args->show_all) {
        dex_show_class();
    }

    return 0;
}
