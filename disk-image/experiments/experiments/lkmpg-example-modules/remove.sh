#!/bin/bash  
shopt -s nullglob

for module in *.ko
do
    echo "removing $module"
    rmmod $module
    echo "removed $module"
done