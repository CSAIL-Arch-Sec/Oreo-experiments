#!/bin/bash 

DIR1=$(ls -t m5outs | head -n1)
DIR2=$(ls -t m5outs/$DIR1 | head -n1)
#echo "$DIR1"
#echo "$DIR2"

if [ ${1:-"foo"} == "debug" ]; then
    FILE="debug.txt"
else
    FILE="board.pc.com_1.device"
fi

tail -f -n100 m5outs/$DIR1/$DIR2/$FILE
