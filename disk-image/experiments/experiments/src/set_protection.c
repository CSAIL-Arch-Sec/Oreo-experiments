#include <sys/syscall.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>


#define PROCFS_NAME "set_protection"


typedef struct set_protection_command_t {
    unsigned long module_delta;
    unsigned long user_delta;
} set_protection_command;


static inline void call_kernel(int kernel_fd, uint64_t module_delta, uint64_t user_delta) {
    set_protection_command local_cmd;
    local_cmd.module_delta = module_delta;
    local_cmd.user_delta = user_delta;

    write(kernel_fd, (void *) &local_cmd, sizeof(local_cmd));
}


int main(int argc, char *argv[]) {
    int kernel_fd;
    uint64_t module_delta = 0, user_delta = 0;

    if (argc > 2) {
        module_delta = strtol(argv[1], NULL, 16);
        user_delta = strtol(argv[2], NULL, 16);
    }

    kernel_fd = open("/proc/" PROCFS_NAME, O_RDWR);
    if (kernel_fd < 0) {
        perror("Problem connecting to the kernel module /proc/%s - did you install it?\n", PROCFS_NAME);
        exit(EXIT_FAILURE);
    }

    call_kernel(kernel_fd, module_delta, user_delta);
}
