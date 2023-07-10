#define M5_RESETSTATS mov $0, %rdi; mov $0, %rsi; .word 0x040F; .word 0x0040
#define M5_DUMPSTATS  mov $0, %rdi; mov $0, %rsi; .word 0x040F; .word 0x0041
