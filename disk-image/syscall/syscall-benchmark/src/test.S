#include "m5ops_x86.h"

.text
.global _start
_start:
#	M5_RESETSTATS

	mov $39, %rax
	syscall

#	M5_DUMPSTATS

	mov $60, %rax
	syscall
	hlt
