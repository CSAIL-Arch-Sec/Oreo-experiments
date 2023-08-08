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
By default, outputs from save and restore runs will be in:
```
experiments/m5outs/default-save/m5out-gen-cpt/
```
```
experiments/m5outs/default-save/m5out-default-restore/
```

## Saving and restoring checkpoints

### Save checkpoint at boot end
```
gem5/build/X86/gem5.opt experiments/gem5-configs/x86-save.py \
    --kernel linux/vmlinux_gem5 \
    --cpu-type KVM
```

### Restore checkpoint from boot end
```
gem5/build/X86/gem5.opt experiments/gem5-configs/x86-restore.py \
    --cpu-type O3 \
```

### Save checkpoint at given ticks
```
gem5/build/X86/gem5.opt experiments/gem5-configs/x86-save.py \
    --kernel linux/vmlinux_gem5 \
    --cpu-type KVM \
    --checkpoint 1000000000000 2000000000000 3000000000000
```

### Restore checkpoint from specific tick checkpoint
```
gem5/build/X86/gem5.opt experiments/gem5-configs/x86-restore.py \
    --cpu-type O3 \
    --checkpoint-tick 1000000000000
```
