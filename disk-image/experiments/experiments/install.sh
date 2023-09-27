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