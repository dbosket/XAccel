#!/usr/bin/env bash

XACCEL="/home/taco/Software/XAccel"


echo "STEP 1: COMPILING KERNEL MODULE AND USER APPLICATION"
pushd "$XACCEL" > /dev/null || exit 1
if ! ( make clean && make && make smoke ); then
	echo "Compilation Failed...Please correct errors"
	exit 1
fi

echo "STEP 2: INSERTING KERNEL MODULE"
if [[ ! -e "$XACCEL/xaccel.ko" ]]; then
	echo "Kernel Module xaccel.ko doesn't exist..."
	exit 1
fi

if  ! lsmod | grep -q '^xaccel\b'; then
	sudo insmod "$XACCEL/xaccel.ko"
else 
	sudo rmmod xaccel && sudo insmod "$XACCEL/xaccel.ko"
fi

sudo dmesg | tail -n 10


echo "STEP 3: Verify device file exists"
if compgen -G "/dev/xaccel*_func*" > /dev/null; then
	sudo chmod 666 /dev/xaccel*_func*
else
	echo "Device File Deesn't exist yet...";
	exit 1
fi


echo "STEP 4: Run Smokescreen Tests"
if [[ ! -x "$XACCEL/tests/smoketest" ]]; then
	echo "ERROR: excutable not found or not executable"
        exit 1	
fi


if [[ -n "${1:-}" ]]; then
        "$XACCEL/tests/smoketest" "$1"
else
        for dev in /dev/xaccel*_func*; do
                [[ -e "$dev" ]] || continue
                echo "Running smoketest on $dev"
                "$XACCEL/tests/smoketest" "$dev" "$1" || exit 1
        done
fi



