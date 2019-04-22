#!/bin/bash

dir_path=/home/black/jz2440/nfs_root

if [[ $1 = "clean" ]] ;then
    make clean
    make -C drv_test clean
    rm $dir_path/drv_test -f
    rm $dir_path/*.ko -f
    
else
    make
    cp *.ko $dir_path
    make -C  drv_test
    cp ./drv_test/drv_test $dir_path
fi
