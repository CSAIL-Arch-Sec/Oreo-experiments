#!/bin/bash

make clean
make hook
make run EXTRA_FLAGS="-D USE_RDTSCP"