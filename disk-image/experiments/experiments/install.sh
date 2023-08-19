mkdir -p bin
gcc -ffreestanding -nostdlib src/test.S -o bin/test

cd modules
make
cd ..
# make