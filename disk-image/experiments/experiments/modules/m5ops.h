#include <linux/types.h>

// https://gcc.gnu.org/onlinedocs/gcc-4.8.5/cpp/Stringification.html
#define __STRINGIFY(s...) #s
#define STRINGIFY(s...) __STRINGIFY(s)

// m5op id values from gem5 source at include/gem5/asm/generic/m5ops.h

#define M5OP_ARM                0x00
#define M5OP_QUIESCE            0x01
#define M5OP_QUIESCE_NS         0x02
#define M5OP_QUIESCE_CYCLE      0x03
#define M5OP_QUIESCE_TIME       0x04
#define M5OP_RPNS               0x07
#define M5OP_WAKE_CPU           0x09
#define M5OP_DEPRECATED1        0x10 // obsolete ivlb
#define M5OP_DEPRECATED2        0x11 // obsolete ivle
#define M5OP_DEPRECATED3        0x20 // deprecated exit function
#define M5OP_EXIT               0x21
#define M5OP_FAIL               0x22
#define M5OP_SUM                0x23 // For testing
#define M5OP_INIT_PARAM         0x30
#define M5OP_LOAD_SYMBOL        0x31
#define M5OP_RESET_STATS        0x40
#define M5OP_DUMP_STATS         0x41
#define M5OP_DUMP_RESET_STATS   0x42
#define M5OP_CHECKPOINT         0x43
#define M5OP_WRITE_FILE         0x4F
#define M5OP_READ_FILE          0x50
#define M5OP_DEBUG_BREAK        0x51
#define M5OP_SWITCH_CPU         0x52
#define M5OP_ADD_SYMBOL         0x53
#define M5OP_PANIC              0x54

#define M5OP_RESERVED1          0x55 // Reserved for user, used to be annotate
#define M5OP_RESERVED2          0x56 // Reserved for user
#define M5OP_RESERVED3          0x57 // Reserved for user
#define M5OP_RESERVED4          0x58 // Reserved for user
#define M5OP_RESERVED5          0x59 // Reserved for user

#define M5OP_WORK_BEGIN         0x5a
#define M5OP_WORK_END           0x5b

#define M5OP_DIST_TOGGLE_SYNC   0x62

#define M5OP_WORKLOAD           0x70

// addr version won't work rn i think ;-;
// would need to get the map m5 mem thing to work first ??
// or actually like todo: maybe i should just link and compile the m5ops the intended way haha

// hardcode magic memory accesses
#define __M5_OP_ADDR(num, ...) __asm__ __volatile__ (__M5_OP_ADDR_ASM_STR(num) : : __VA_ARGS__ : "%r11", "%rax")
#define __M5_OP_ADDR_ASM(num)   movq $0xFFFF0000, %%r11; \
                                movq $ ## num, %%rax; shl $8, %%rax; \
                                movq 0(%%r11, %%rax, 1), %%rax;
#define __M5_OP_ADDR_ASM_STR(num) STRINGIFY(__M5_OP_ADDR_ASM(num))
#define M5_OP_ADDR(num, ...) __M5_OP_ADDR(num, __VA_ARGS__)

// some of them return things i don't think this hack can really do ;-;
#define M5_ARM_ADDR(address)                                M5_OP_ADDR(M5OP_ARM, "D" (address))
#define M5_QUIESCE_ADDR()                                   M5_OP_ADDR(M5OP_QUIESCE)
#define M5_QUIESCE_NS_ADDR(ns)                              M5_OP_ADDR(M5OP_QUIESCE_NS, "D" (ns))
#define M5_QUIESCE_CYCLE_ADDR(cycles)                       M5_OP_ADDR(M5OP_QUIESCE_CYCLE, "D" (cycles))
// #define M5_QUIESCE_TIME_ADDR()                              M5_OP_ADDR(M5OP_QUIESCE_TIME)
// #define M5_RPNS_ADDR()                                      M5_OP_ADDR(M5OP_RPNS)
#define M5_WAKE_CPU_ADDR(cpuid)                             M5_OP_ADDR(M5OP_WAKE_CPU, "D" (cpuid))
#define M5_EXIT_ADDR(ns_delay)                              M5_OP_ADDR(M5OP_EXIT, "D" (ns_delay))
#define M5_FAIL_ADDR(ns_delay, code)                        M5_OP_ADDR(M5OP_FAIL, "D" (ns_delay), "S" (code))
// #define M5_SUM_ADDR(a, b, c, d, e, f)                       M5_OP_ADDR(M5OP_SUM, )
// #define M5_INIT_PARAM_ADDR(key_str1, key_str2)              M5_OP_ADDR(M5OP_INIT_PARAM, "D" (key_str1), "S" (key_str2))
#define M5_CHECKPOINT_ADDR(ns_delay, ns_period)             M5_OP_ADDR(M5OP_CHECKPOINT, "D" (ns_delay), "S" (ns_period))
#define M5_RESET_STATS_ADDR(ns_delay, ns_period)            M5_OP_ADDR(M5OP_RESET_STATS, "D" (ns_delay), "S" (ns_period))
#define M5_DUMP_STATS_ADDR(ns_delay, ns_period)             M5_OP_ADDR(M5OP_DUMP_STATS, "D" (ns_delay), "S" (ns_period))
#define M5_DUMP_RESET_STATS_ADDR(ns_delay, ns_period)       M5_OP_ADDR(M5OP_DUMP_RESET_STATS, "D" (ns_delay), "S" (ns_period))
// #define M5_READ_FILE_ADDR(buffer, len, offset)              M5_OP_ADDR(M5OP_READ_FILE, "D" (buffer), "S" (len), "d" (offset))
// #define M5_WRITE_FILE_ADDR(buffer, len, offset, filename)   M5_OP_ADDR(M5OP_WRITE_FILE, "D" (buffer), "S" (len), "d" (offset), "c" (filename))
#define M5_DEBUG_BREAK_ADDR()                               M5_OP_ADDR(M5OP_DEBUG_BREAK)
#define M5_SWITCH_CPU_ADDR()                                M5_OP_ADDR(M5OP_SWITCH_CPU)
#define M5_DIST_TOGGLE_SYNC_ADDR()                          M5_OP_ADDR(M5OP_DIST_TOGGLE_SYNC)
#define M5_ADD_SYMBOL_ADDR(addr, symbol)                    M5_OP_ADDR(M5OP_ADD_SYMBOL, "D" (addr), "S" (symbol))
#define M5_LOAD_SYMBOL_ADDR()                               M5_OP_ADDR(M5OP_LOAD_SYMBOL)
#define M5_PANIC_ADDR()                                     M5_OP_ADDR(M5OP_PANIC)
#define M5_WORK_BEGIN_ADDR(workid, threadid)                M5_OP_ADDR(M5OP_WORK_BEGIN, "D" (workid), "S" (threadid))
#define M5_WORK_END_ADDR(workid, threadid)                  M5_OP_ADDR(M5OP_WORK_END, "D" (workid), "S" (threadid))
#define M5_WORKLOAD_ADDR()                                  M5_OP_ADDR(M5OP_WORKLOAD)


// hardcode magic instructions 
#define __M5_OP_INST(num, out, ...) __asm__ __volatile__ (__M5_OP_INST_ASM_STR(num) : out : __VA_ARGS__ :)
#define __M5_OP_INST_ASM(num) .word 0x040F; .word num;
#define __M5_OP_INST_ASM_STR(num) STRINGIFY(__M5_OP_INST_ASM(num))
#define M5_OP_INST(num, ...) __M5_OP_INST(num, , __VA_ARGS__)
#define M5_OP_INST_RET(num, ...) __M5_OP_INST(num, "=r" (_rax), __VA_ARGS__)

// some of them return things i don't think this hack can really do ;-;
#define M5_ARM_INST(address)                                M5_OP_INST(M5OP_ARM, "D" (address))
#define M5_QUIESCE_INST()                                   M5_OP_INST(M5OP_QUIESCE)
#define M5_QUIESCE_NS_INST(ns)                              M5_OP_INST(M5OP_QUIESCE_NS, "D" (ns))
#define M5_QUIESCE_CYCLE_INST(cycles)                       M5_OP_INST(M5OP_QUIESCE_CYCLE, "D" (cycles))
#define M5_QUIESCE_TIME_INST()                              M5_OP_INST_RET(M5OP_QUIESCE_TIME)
#define M5_RPNS_INST()                                      M5_OP_INST_RET(M5OP_RPNS)
#define M5_WAKE_CPU_INST(cpuid)                             M5_OP_INST(M5OP_WAKE_CPU, "D" (cpuid))
#define M5_EXIT_INST(ns_delay)                              M5_OP_INST(M5OP_EXIT, "D" (ns_delay))
#define M5_FAIL_INST(ns_delay, code)                        M5_OP_INST(M5OP_FAIL, "D" (ns_delay), "S" (code))
// #define M5_SUM_INST(a, b, c, d, e, f)                       M5_OP_INST(M5OP_SUM, )
#define M5_INIT_PARAM_INST(key_str1, key_str2)              M5_OP_INST_RET(M5OP_INIT_PARAM, "D" (key_str1), "S" (key_str2))
#define M5_CHECKPOINT_INST(ns_delay, ns_period)             M5_OP_INST(M5OP_CHECKPOINT, "D" (ns_delay), "S" (ns_period))
#define M5_RESET_STATS_INST(ns_delay, ns_period)            M5_OP_INST(M5OP_RESET_STATS, "D" (ns_delay), "S" (ns_period))
#define M5_DUMP_STATS_INST(ns_delay, ns_period)             M5_OP_INST(M5OP_DUMP_STATS, "D" (ns_delay), "S" (ns_period))
#define M5_DUMP_RESET_STATS_INST(ns_delay, ns_period)       M5_OP_INST(M5OP_DUMP_RESET_STATS, "D" (ns_delay), "S" (ns_period))
#define M5_READ_FILE_INST(buffer, len, offset)              M5_OP_INST_RET(M5OP_READ_FILE, "D" (buffer), "S" (len), "d" (offset))
#define M5_WRITE_FILE_INST(buffer, len, offset, filename)   M5_OP_INST_RET(M5OP_WRITE_FILE, "D" (buffer), "S" (len), "d" (offset), "c" (filename))
#define M5_DEBUG_BREAK_INST()                               M5_OP_INST(M5OP_DEBUG_BREAK)
#define M5_SWITCH_CPU_INST()                                M5_OP_INST(M5OP_SWITCH_CPU)
#define M5_DIST_TOGGLE_SYNC_INST()                          M5_OP_INST(M5OP_DIST_TOGGLE_SYNC)
#define M5_ADD_SYMBOL_INST(addr, symbol)                    M5_OP_INST(M5OP_ADD_SYMBOL, "D" (addr), "S" (symbol))
#define M5_LOAD_SYMBOL_INST()                               M5_OP_INST(M5OP_LOAD_SYMBOL)
#define M5_PANIC_INST()                                     M5_OP_INST(M5OP_PANIC)
#define M5_WORK_BEGIN_INST(workid, threadid)                M5_OP_INST(M5OP_WORK_BEGIN, "D" (workid), "S" (threadid))
#define M5_WORK_END_INST(workid, threadid)                  M5_OP_INST(M5OP_WORK_END, "D" (workid), "S" (threadid))
#define M5_WORKLOAD_INST()                                  M5_OP_INST(M5OP_WORKLOAD)


// pick one to use i guess
#define M5_ARM(address)                                 M5_ARM_INST(address)
#define M5_QUIESCE()                                    M5_QUIESCE_INST()
#define M5_QUIESCE_NS(ns)                               M5_QUIESCE_NS_INST(ns)
#define M5_QUIESCE_CYCLE(cycles)                        M5_QUIESCE_CYCLE_INST(cycles)
#define M5_QUIESCE_TIME()                               M5_QUIESCE_TIME_INST()
#define M5_RPNS()                                       M5_RPNS_INST()
#define M5_WAKE_CPU(cpuid)                              M5_WAKE_CPU_INST(cpuid)
#define M5_EXIT(ns_delay)                               M5_EXIT_INST(ns_delay)
#define M5_FAIL(ns_delay, code)                         M5_FAIL_INST(ns_delay, code)
// #define M5_SUM(a, b, c, d, e, f)                        M5_SUM_INST(a, b, c, d, e, f)
#define M5_INIT_PARAM(key_str1, key_str2)               M5_INIT_PARAM_INST(key_str1, key_str2)
#define M5_CHECKPOINT(ns_delay, ns_period)              M5_CHECKPOINT_INST(ns_delay, ns_period)
#define M5_RESET_STATS(ns_delay, ns_period)             M5_RESET_STATS_INST(ns_delay, ns_period)
#define M5_DUMP_STATS(ns_delay, ns_period)              M5_DUMP_STATS_INST(ns_delay, ns_period)
#define M5_DUMP_RESET_STATS(ns_delay, ns_period)        M5_DUMP_RESET_STATS_INST(ns_delay, ns_period)
#define M5_READ_FILE(buffer, len, offset)               M5_READ_FILE_INST(buffer, len, offset)
#define M5_WRITE_FILE(buffer, len, offset, filename)    M5_WRITE_FILE_INST(buffer, len, offset, filename)
#define M5_DEBUG_BREAK()                                M5_DEBUG_BREAK_INST()
#define M5_SWITCH_CPU()                                 M5_SWITCH_CPU_INST()
#define M5_DIST_TOGGLE_SYNC()                           M5_DIST_TOGGLE_SYNC_INST()
#define M5_ADD_SYMBOL(addr, symbol)                     M5_ADD_SYMBOL_INST(addr, symbol)
#define M5_LOAD_SYMBOL()                                M5_LOAD_SYMBOL_INST()
#define M5_PANIC()                                      M5_PANIC_INST()
#define M5_WORK_BEGIN(workid, threadid)                 M5_WORK_BEGIN_INST(workid, threadid)
#define M5_WORK_END(workid, threadid)                   M5_WORK_END_INST(workid, threadid)
#define M5_WORKLOAD()                                   M5_WORKLOAD_INST()

#define M5_FUNC_RET(inline_asm) uint64_t _rax; inline_asm; return _rax;

static inline void m5_arm(uint64_t address) { 
    M5_ARM(address); 
}
static inline void m5_quiesce(void) {
    M5_QUIESCE(); 
}
static inline void m5_quiesce_ns(uint64_t ns) { 
    M5_QUIESCE_NS(ns); 
}
static inline void m5_quiesce_cycle(uint64_t cycles) { 
    M5_QUIESCE_CYCLE(cycles); 
}
static inline uint64_t m5_quiesce_time(void) { 
    M5_FUNC_RET(M5_QUIESCE_TIME()) 
}
static inline uint64_t m5_rpns(void) {
    M5_FUNC_RET(M5_RPNS())
}
static inline void m5_wake_cpu(uint64_t cpuid) { 
    M5_WAKE_CPU(cpuid); 
}

static inline void m5_exit(uint64_t ns_delay) { 
    M5_EXIT(ns_delay); 
}
static inline void m5_fail(uint64_t ns_delay, uint64_t code) { 
    M5_FAIL(ns_delay, code); 
}
// unsigned m5_sum(unsigned a, unsigned b, unsigned c,
//                 unsigned d, unsigned e, unsigned f);
static inline uint64_t m5_init_param(uint64_t key_str1, uint64_t key_str2) { 
    M5_FUNC_RET(M5_INIT_PARAM(key_str1, key_str2)) 
}
static inline void m5_checkpoint(uint64_t ns_delay, uint64_t ns_period) { 
    M5_CHECKPOINT(ns_delay, ns_period); 
}
static inline void m5_reset_stats(uint64_t ns_delay, uint64_t ns_period) { 
    M5_RESET_STATS(ns_delay, ns_period); 
}
static inline void m5_dump_stats(uint64_t ns_delay, uint64_t ns_period) { 
    M5_DUMP_STATS(ns_delay, ns_period); 
}
static inline void m5_dump_reset_stats(uint64_t ns_delay, uint64_t ns_period) { 
    M5_DUMP_RESET_STATS(ns_delay, ns_period); 
}
static inline uint64_t m5_read_file(void *buffer, uint64_t len, uint64_t offset) { 
    M5_FUNC_RET(M5_READ_FILE(buffer, len, offset)) 
}
static inline uint64_t m5_write_file(void *buffer, uint64_t len, uint64_t offset,
                       const char *filename) { 
    M5_FUNC_RET(M5_WRITE_FILE(buffer, len, offset, filename)) 
}
static inline void m5_debug_break(void) { 
    M5_DEBUG_BREAK(); 
}
static inline void m5_switch_cpu(void) { 
    M5_SWITCH_CPU(); 
}
static inline void m5_dist_toggle_sync(void) { 
    M5_DIST_TOGGLE_SYNC(); 
}
static inline void m5_add_symbol(uint64_t addr, const char *symbol) { 
    M5_ADD_SYMBOL(addr, symbol); 
}
static inline void m5_load_symbol(void) {
    M5_LOAD_SYMBOL(); 
}
static inline void m5_panic(void) { 
    M5_PANIC(); 
}
static inline void m5_work_begin(uint64_t workid, uint64_t threadid) { 
    M5_WORK_BEGIN(workid, threadid); 
}
static inline void m5_work_end(uint64_t workid, uint64_t threadid) { 
    M5_WORK_END(workid, threadid); 
}
static inline void m5_workload(void) {
    M5_WORKLOAD(); 
}