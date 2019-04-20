
BUSYBOX_NAME=busybox-1.7.0
PATCH_NAME=busybox_1.7_patch

# remove old busybox
rm ${BUSYBOX_NAME} -rf

# uncompress
tar -xf ${BUSYBOX_NAME}.tar.bz2

cd ${BUSYBOX_NAME}
patch -p1  < ../${PACTH_NAME}
make menuconfig
make
