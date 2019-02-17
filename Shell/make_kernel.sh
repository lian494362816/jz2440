#!/bin/sh

rm linux-2.6.22.6 -r
tar -xf linux-2.6.22.6.tar.bz2
cd linux-2.6.22.6
patch -p1 <../linux-2.6.22.6_jz2440.patch
cp ../4.3_LCD_mach-smdk2440.c arch/arm/mach-s3c2440/mach-smdk2440.c
cp config_ok .config

sed -i "s/config %config/%config/g" Makefile
sed -i "s/\/ %\/: prepare/%\/: prepare/g" Makefile

make uImage -j4
