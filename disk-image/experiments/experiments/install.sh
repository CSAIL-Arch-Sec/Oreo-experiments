mkdir -p bin
gcc -ffreestanding -nostdlib src/test.S -o bin/test
mkdir -p modules
# make