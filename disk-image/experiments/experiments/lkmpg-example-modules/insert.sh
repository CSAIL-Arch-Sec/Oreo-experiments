#!/bin/bash  
shopt -s nullglob

for module in *.ko
do
    if [[ $module == "bottomhalf.ko" || $module == "intrpt.ko" ]]; then
      echo "skipping $module"
      continue
    fi
    echo "inserting $module"
    insmod $module
    echo "inserted $module"
done