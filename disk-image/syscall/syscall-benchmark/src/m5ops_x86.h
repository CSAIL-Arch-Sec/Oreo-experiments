#define M5_RESET_STATS \
    mov $0, %rdi \
    mov $0, %rsi \
    .byte 0x0F, 0x04 \
    .word 0x40

#define M5_DUMP_STATS \
    mov $0, %rdi \
    mov $0, %rsi \
    .byte 0x0F, 0x04 \
    .word 0x41