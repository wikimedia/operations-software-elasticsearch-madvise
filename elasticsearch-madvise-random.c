/**
 * from https://phabricator.wikimedia.org/P5883 (GPL-2-or-later)
 * Requires two libraries:
 * - https://github.com/emptymonkey/ptrace_do
 * - https://github.com/ouadev/proc_maps_parser
 */

#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "ptrace_do/libptrace_do.h"
#include "proc_maps_parser/pmparser.h"

void check_madvise(int res, char *path) {
    if (!res) {
        printf("success : %s\n", path);
    } else {
        switch (errno) {
            case EACCES:
                printf("EACCES: %s\n", path);
                break;
            case EAGAIN:
                printf("EAGAIN: %s\n", path);
                break;
            case EBADF:
                printf("EBADF: %s\n", path);
                break;
            case EINVAL:
                printf("EINVAL: %s\n", path);
                break;
            case EIO:
                printf("EIO: %s\n", path);
                break;
            case ENOMEM:
                printf("ENOMEM: %s\n", path);
                break;
            case EPERM:
                printf("EPERM: %s\n", path);
                break;
            default:
                printf("unknown %d: %s\n", errno, path);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("USAGE: %s <pid>\n", argv[0]);
        return -1;
    }

    pid_t pid = atoi(argv[1]);
    printf("breaking pid %d\n", pid);

    procmaps_struct *maps = pmparser_parse(pid);
    if (maps == NULL) {
        printf("cannot parser the memory map of %d\n", pid);
        return -1;
    }

    struct ptrace_do *target = ptrace_do_init(pid);
    procmaps_struct *maps_tmp = NULL;
    char *prefix = "/srv/elasticsearch";
    while ((maps_tmp = pmparser_next()) != NULL) {
        if (strncmp(prefix, maps_tmp->pathname, strlen(prefix)) == 0) {
            int res = ptrace_do_syscall(
                target,
                __NR_madvise,
                (unsigned long int) maps_tmp->addr_start,
                maps_tmp->length,
                MADV_RANDOM,
                0, 0, 0);
            check_madvise(res, maps_tmp->pathname);
        }
    }

    ptrace_do_cleanup(target);

    return 0;
}
