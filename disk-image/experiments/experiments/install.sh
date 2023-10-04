#!/bin/bash

make clean
make all

cd modules
make clean
make all
cd ..

cd lkmpg-example-modules
make clean
make all
cd ..

cd spectre-jz268
make clean
make all
cd module-src
make clean 
make
cd ..
cd ..

cd blindside
make -B
cd module-src
make -B
cd ../..