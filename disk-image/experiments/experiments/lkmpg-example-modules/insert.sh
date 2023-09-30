#!/bin/bash  
shopt -s nullglob

for module in *.ko
do
    echo "inserting $module"
    insmod $module
    chmod -R 777 /proc/$module
    echo "inserted $module"
done