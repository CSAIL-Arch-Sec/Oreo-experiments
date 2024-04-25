#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <x86intrin.h>
#include <sys/mman.h>
#include "m5ops.h"

int main() {
    m5_work_begin(0, 0);
    printf("Trigger system call\n");
    m5_work_end(0, 0);
    return 0;
}
