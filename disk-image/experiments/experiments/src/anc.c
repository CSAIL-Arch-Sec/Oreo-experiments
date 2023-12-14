//
// Created by shixinsong on 10/5/23.
//
//
// Created by shixinsong on 10/2/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include "dummy.h"
#include "m5ops.h"

#define PROCFS_NAME "blindside-victim"
#define START_KERNEL 0xffffffff80000000
#define ALIGN 0x2000000
#define NUM_OFFSETS 32


char eviction_buffer[64 * 1024]; // 64KB
void prime_l1() {
    uint64_t i = 0;
    for (i = 0; i < 1024; i++) {
        eviction_buffer[i * 64]++;
    }
}


void flush_itlb() {
    uint64_t i = 0;
    for (i = 0; i < sizeof(func) / 8; i++) {
        func[i](i, 1);
    }
}

static inline void clflush(volatile void *__p)
{
    asm volatile("clflush %0" : "+m" (*(volatile char *)__p));
}

int foo_x = 0;
int foo_y = 1;

int test_foo(void) {
    if (foo_x == foo_y) {
        printf("test_foo\n");
    }

    return (foo_x * 353 - foo_y % 23) * foo_x / foo_y;
}

static volatile unsigned long __attribute__((aligned(32768))) foo_ptr = 0;

//inline void jump_to_address(void* address) {
//    asm volatile (
//            "call *%0"  // Indirect call to the address pointed to by %0
//            :
//            : "r" (address)
//            : "memory" // clobber memory to prevent compiler optimizations
//            );
//}

int main() {
    test_foo();

    int kernel_fd = open("/proc/" PROCFS_NAME, O_RDWR);
    if (kernel_fd < 0) {
        perror("Problem connecting to the kernel module- did you install it?\n");
        exit(EXIT_FAILURE);
    }

    void *f, *g;

    flush_itlb();
    prime_l1();

    foo_ptr = (unsigned long) test_foo;
    clflush(&foo_ptr);
    g = (void *) foo_ptr;
    asm volatile (
            "call *%0"  // Indirect call to the address pointed to by %0
            :
            : "r" (g)
            : "memory" // clobber memory to prevent compiler optimizations
            );
//    m5_work_begin(0, 0);
//    syscall(104);
    read(kernel_fd, NULL, 0);
//    m5_work_end(0, 0);
}
