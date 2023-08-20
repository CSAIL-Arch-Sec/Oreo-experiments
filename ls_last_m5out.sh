#!/bin/bash 

DIR1=$(ls -t m5outs | head -n1)
DIR2=$(ls -t m5outs/$DIR1 | head -n1)
#echo "$DIR1"
#echo "$DIR2"

ls  m5outs/$DIR1/$DIR2
