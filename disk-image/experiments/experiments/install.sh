#!/bin/bash

cd src
python3 generate_large_header.py
cd ..

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
