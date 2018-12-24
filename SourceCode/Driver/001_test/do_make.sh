#!/bin/bash

dir_path=/home/black/jz2440/nfs_root

if [[ $1 = "clean" ]] ;then
    make clean
    rm test -f
    rm $dir_path/test -f
    rm $dir_path/*.ko -f
    
else
    make
    cp *.ko $dir_path

    arm-linux-gcc test.c -o test
    cp test $dir_path
fi
