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
(at the moment some of the default paths might be assuming running within the docker container setup in protect-kaslr-linux, need to fix that oop)

## Generate disk image
right now it's set up to first generate a "base" image then install whatever in a second stage so installing from iso doesn't need to happen every time
```
cd experiments/disk-image
./build.sh base-ubuntu
./build.sh experiments
```
(after the first time building base-ubuntu isn't necessary)

if an image is already built, running the build again will make it complain if it's not removed first (i haven't gotten around to fixing that yet oops)

everything in disk-image/experiments/experiments/ will be put on the image (which the save/restore scripts currently use by default, but you can also specify your own image to use)

## Saving and restoring checkpoints
By default, outputs from save and restore runs will be in:
```
experiments/m5outs/default-save/m5out-gen-cpt/
```
```
experiments/m5outs/default-save/m5out-default-restore/
```
there are also a bunch of random options most of which don't really need to be set, mainly just cpu type

can run something like
```
gem5/build/X86/gem5.opt experiments/gem5-configs/x86-save.py --help
```
```
gem5/build/X86/gem5.opt experiments/gem5-configs/x86-restore.py --help
```
to see available arguments (sorry it's kinda not ideal to read oop)

### Save checkpoint at boot end
```
gem5/build/X86/gem5.opt experiments/gem5-configs/x86-save.py \
    --kernel linux/vmlinux_gem \
    --cpu-type KVM
```

### Restore checkpoint from boot end
```
gem5/build/X86/gem5.opt experiments/gem5-configs/x86-restore.py \
    --cpu-type O3
```

### Save checkpoint at given ticks
this takes checkpoints at ticks 1000000000000, 2000000000000, 3000000000000:
```
gem5/build/X86/gem5.opt experiments/gem5-configs/x86-save.py \
    --kernel linux/vmlinux_gem5 \
    --cpu-type KVM \
    --checkpoint 1000000000000 2000000000000 3000000000000
```
### Save checkpoint at given ticks periodically
this takes checkpoints at ticks 1000000000000, 2000000000000, ..., 10000000000000:
```
gem5/build/X86/gem5.opt experiments/gem5-configs/x86-save.py \
    --kernel linux/vmlinux_gem5 \
    --cpu-type KVM \
    --checkpoint 1000000000000,1000000000000,10
```
X,Y,Z means X, X+Y, X+2Y, ..., X+(Z-1)Y

can also have both single and periodic (e.g. --checkpoint 1 2 3,4,5 6,7,8 100 is valid)
### Restore checkpoint from specific tick checkpoint
```
gem5/build/X86/gem5.opt experiments/gem5-configs/x86-restore.py \
    --cpu-type O3 \
    --checkpoint-tick 1000000000000
```
