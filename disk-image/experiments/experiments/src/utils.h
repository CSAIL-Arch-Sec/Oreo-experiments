//
// Created by shixinsong on 10/10/23.
//

#ifndef EXPERIMENTS_UTILS_H
#define EXPERIMENTS_UTILS_H

inline void jump_to_address(void* address) {
    asm volatile (
            "call *%0"  // Indirect call to the address pointed to by %0
            :
            : "r" (address)
            : "memory" // clobber memory to prevent compiler optimizations
            );
}

#endif //EXPERIMENTS_UTILS_H
