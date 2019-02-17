#!/bin/bash
 
if [[ $1 = "tar" ]]; then
    tar -czf nfs_root.tar.gz nfs_root
    mv nfs_root.tar.gz TAR
else
    # echo PS1=\'[board2440 \\w]#\' > nfs_root/etc/profile

    mkyaffs2image nfs_root nfs_root.yaffs2
    mv nfs_root.yaffs2 ./bin

    mkfs.jffs2 -n -s 2048 -e 128KiB -d nfs_root -o nfs_root.jffs2
    mv nfs_root.jffs2 ./bin

    # echo PS1=\'[nfs2440 \\w]#\' > nfs_root/etc/profile
fi

