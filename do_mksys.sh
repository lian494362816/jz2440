#!/bin/sh
 
mkyaffs2image nfs_root nfs_root.yaffs2
mv nfs_root.yaffs2 ./bin

mkfs.jffs2 -n -s 2048 -e 128KiB -d nfs_root -o nfs_root.jffs2
mv nfs_root.jffs2 ./bin
