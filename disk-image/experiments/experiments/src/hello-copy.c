#include <stdio.h>
#include <unistd.h>
#include "m5ops.h"

int main() {
   printf("hello world\n");
   // M5_RESET_STATS(0, 0);
   // M5_DUMP_STATS(0, 0);
   for (int i = 0; i < 10; ++i) {
      printf("round %d of running getpid\n", i);
      M5_WORK_BEGIN(i, 0);
      getpid();
      M5_WORK_END(i, 0);
   }
   printf("exiting simulation ...\n");
   // M5_RESET_STATS(0, 0);
   // M5_DUMP_STATS(0, 0);
   M5_EXIT(0);
}