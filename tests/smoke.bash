#!/usr/bin/env bash

XACCEL="/home/taco/Software/XAccel"
USE_VALGRIND=${USE_VALGRIND:-0}

if [[ "$USE_VALGRIND" == "1" ]]; then
        RUN=(valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=99)
else
        RUN=()
fi

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
        echo "ERROR: executable not found or not executable"
        exit 1
fi

expect_fail()
{
        local name="$1"
        shift

        echo "[NEGATIVE TEST] $name"

        if "${RUN[@]}" "$@"; then
                echo "[FAIL] $name unexpectedly passed"
                exit 1
        else
                echo "[PASS] $name failed as expected"
        fi
}

for dev in /dev/xaccel*_func*; do
        [[ -e "$dev" ]] || continue

        echo "----------------------------------------"
        echo "Running smoketest on $dev"
        echo "----------------------------------------"

        echo "[TEST] GET_INFO"
        "${RUN[@]}" "$XACCEL/tests/smoketest" "$dev" info || exit 1

        echo "[TEST] WRITE_REG offset=0x0 value=0xdeadbeef"
        "${RUN[@]}" "$XACCEL/tests/smoketest" "$dev" write 0x0 0xdeadbeef || exit 1

        echo "[TEST] READ_REG offset=0x0"
        "${RUN[@]}" "$XACCEL/tests/smoketest" "$dev" read 0x0 || exit 1

        echo "[TEST] WRITE_REG offset=0x4 value=0xcafebabe"
        "${RUN[@]}" "$XACCEL/tests/smoketest" "$dev" write 0x4 0xcafebabe || exit 1

        echo "[TEST] READ_REG offset=0x4"
        "${RUN[@]}" "$XACCEL/tests/smoketest" "$dev" read 0x4 || exit 1

        echo "[TEST] READ_REG boundary offset=0x1fc"
        "${RUN[@]}" "$XACCEL/tests/smoketest" "$dev" read 0x1fc || exit 1

        echo "[TEST] Negative/error-path cases"

        expect_fail "READ_REG out-of-range offset=0x200" \
                "$XACCEL/tests/smoketest" "$dev" read 0x200

        expect_fail "WRITE_REG out-of-range offset=0x200" \
                "$XACCEL/tests/smoketest" "$dev" write 0x200 0x12345678

        expect_fail "READ_REG unaligned offset=0x1" \
                "$XACCEL/tests/smoketest" "$dev" read 0x1

        expect_fail "WRITE_REG unaligned offset=0x1" \
                "$XACCEL/tests/smoketest" "$dev" write 0x1 0x12345678

        expect_fail "Invalid command" \
                "$XACCEL/tests/smoketest" "$dev" badcmd

        echo "[PASS] Smoketest passed for $dev"
done
