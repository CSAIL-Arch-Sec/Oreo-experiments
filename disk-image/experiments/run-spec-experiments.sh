#!/bin/sh

# We need to run each benchmark to generate the run folder.
# We do not need to finish the experiments to be done.

cd /home/gem5/spec2017
. ./shrc

for bench in 500.perlbench_r 502.gcc_r 505.mcf_r 520.omnetpp_r 523.xalancbmk_r 525.x264_r 531.deepsjeng_r 541.leela_r 548.exchange2_r 557.xz_r 999.specrand_ir
do
  echo ${bench}
  timeout 10 runcpu --size ref --iterations 1 --config myconfig.x86.cfg --define gcc_dir="/usr" --noreportable --nobuild ${bench}
done

rm -f /home/gem5/spec2017/result/*

