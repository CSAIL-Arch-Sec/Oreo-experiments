#include <stdio.h>
#include <unistd.h>
#include "m5ops.h"

int main() {
   printf("hello world\n");
   printf("now exiting hopefully ...\n");
   for (int i = 0; i < 10; ++i) {
      printf("round %d of running getpid\n", i);
      M5_RESET_STATS(0, 0);
      getpid();
      M5_DUMP_STATS(0, 0);
   }
   printf("exiting simulation ...")
   M5_EXIT(0);
}