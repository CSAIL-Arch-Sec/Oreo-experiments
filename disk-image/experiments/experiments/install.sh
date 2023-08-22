#!/bin/bash

make clean
make all

cd modules
make clean
make all
cd ..