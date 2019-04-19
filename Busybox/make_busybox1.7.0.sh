
BUSYBOX_NAME=busybox-1.7.0
PATH_NAME=busybox_1.7_patch

# remove old busybox
rm ${BUSYBOX_NAME} -rf

# uncompress
tar -xf ${BUSYBOX_NAME}.tar.bz2

cd ${BUSYBOX_NAME}
patch -p1  < ../${PATH_NAME}
# make menuconfig
make
