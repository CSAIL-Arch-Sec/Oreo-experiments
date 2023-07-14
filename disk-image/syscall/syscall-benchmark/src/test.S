#include "m5ops/m5ops.h"

.text
.global _start

_start:

	M5_DUMP_STATS

	mov $39, %rax
	syscall

	M5_RESET_STATS

	mov $60, %rax
	syscall
	hlt
