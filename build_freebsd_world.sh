#!/bin/sh

# Set environment variables
export DESTDIR=/home/an683/freebsd-riscv-world/
export MAKEOBJDIRPREFIX=/home/an683/obj/
export SRCCONF=/home/an683/freebsd-riscv/etc/src.conf
unset CC; unset CXX

# Build FreeBSD world
make -j16 SRCCONF=$SRCCONF KERNCONF=RV64IMAC TARGET_ARCH=riscv64sf buildworld

# Install FreeBSD world
make -j16 SRCCONF=$SRCCONF KERNCONF=RV64IMAC TARGET_ARCH=riscv64sf -DNO_ROOT DESTDIR=$DESTDIR installworld

# Distribute FreeBSD world
make SRCCONF=$SRCCONF KERNCONF=RV64IMAC TARGET_ARCH=riscv64sf -DNO_ROOT DESTDIR=$DESTDIR distribution

