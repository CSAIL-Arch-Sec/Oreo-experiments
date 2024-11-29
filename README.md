# Generate Disk Image with Vagrant

## File structure
```
artifact
    | experiments # This project
    | gem5
    | linux
    | cpu2017-1.1.9.iso
    | ...
```
Note: `cpu2017-1.1.9.iso` is the official image for SPEC 2017 benchmarks.
It will not be released with the final artifact since it is not open-source.

## Prerequisite
Before started, ensure that
1. `Vagrant` and `VirtualBox` are installed on the host machine (we do not use docker to build the disk image).
2. `gem5/util/m5/build/x86/out/m5` is built.
3. `linux/vmlinux_gem5` is built.

In the artifact root directory, pack Linux source files for faster disk image build.
```bash
tar -cvf linux.tar linux
```

## Generate Disk Image
Run the following commands to generate the disk image, which takes about 1 hour on an Intel(R) Xeon(R) Gold 5220R CPU.
```bash
cd experiments/disk-image
vagrant up
vagrant halt # To shut down the current VM
vagrant global-status --prune # To shut down other VMs
```

Next, we convert VirtualBox image to the raw image for gem5
```bash
ls ~/VirtualBox\ VMs
# Find vm directory denoted as <vm-directory>
cd experiments/disk-image
qemu-img convert -f vmdk -O raw ~/VirtualBox\ VMs/<vm-directory>/ubuntu-jammy-22.04-cloudimg.vmdk experiments.img
```
