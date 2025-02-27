#!/bin/sh

# Set environment variables
export DESTDIR="/home/an683/freebsd-riscv-world/"
export MAKEOBJDIRPREFIX="/home/an683/obj/"
export SRCCONF="/home/an683/freebsd-riscv/etc/src.conf"

unset CC; unset CXX

# Create mdroot.img
echo "Creating root filesystem image (mdroot.img)..."
./tools/tools/makeroot/makeroot.sh -s 64m -f basic.files mdroot.img $DESTDIR

# Kernel build settings
KERNCONF="RV64IMAC"
TARGET_ARCH="riscv64sf"
JOBS=16

# Paths
FREEBSD_DIR="$HOME/freebsd-riscv"
OBJ_DIR="/home/an683/obj/usr/home/an683/freebsd-riscv/riscv.riscv64sf/sys/$KERNCONF"
KERNEL_BIN="$OBJ_DIR/kernel.bin"
OPENSBI_DIR="$HOME/opensbi"

# Build FreeBSD Kernel
echo "Building FreeBSD kernel..."
cd "$FREEBSD_DIR" || exit
make -j$JOBS SRCCONF=$SRCCONF KERNCONF=$KERNCONF TARGET_ARCH=$TARGET_ARCH buildkernel
make -j$JOBS SRCCONF=$SRCCONF KERNCONF=$KERNCONF TARGET_ARCH=$TARGET_ARCH -DNO_ROOT DESTDIR=$DESTDIR installkernel

# Convert Kernel to Binary
echo "Converting kernel to binary..."
riscv64-unknown-elf-objcopy -O binary "$OBJ_DIR/kernel" "$KERNEL_BIN"

# Build OpenSBI with Kernel Payload
echo "Building OpenSBI with FreeBSD kernel payload..."
cd "$OPENSBI_DIR" || exit
gmake -j$JOBS PLATFORM=generic FW_PAYLOAD_PATH="$KERNEL_BIN"

echo "Build process completed successfully!"

