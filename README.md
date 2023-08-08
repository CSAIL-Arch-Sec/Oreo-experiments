# protect-kaslr-experiments

## File structure
Please put this linux repo and a gem5 repo under the same directory. Here is an example layout
```
protect-kaslr/
  |____ linux/          # linux source code
  |
  |____ gem5/           # gem5 source code
  |
  |____ experiments/    # experiments source code
```

## Save checkpoint
From the protect-kaslr directory
Example:
```
gem5/build/X86/gem5.opt experiments/gem5-configs/x86-save.py --kernel linux/vmlinux_gem5 --cpu-type KVM --checkpoint 1000000000000
```

## Restore checkpoint
From the protect-kaslr directory
Example:
```
gem5/build/X86/gem5.opt experiments/gem5-configs/x86-restore.py --cpu-type TIMING --checkpoint-tick 1000000000000
```
