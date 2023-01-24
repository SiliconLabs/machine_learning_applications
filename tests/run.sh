#!/bin/sh
# Usage:
# path/to/run.sh path/to/gecko_sdk

set -e
set -x

SCRIPT_PATH=$(readlink -f "$0")
TESTS_DIR=$(dirname "$SCRIPT_PATH")
BUILD_DIR="$TESTS_DIR/build"

cmake -S $TESTS_DIR -B $TESTS_DIR/build -DGSDK_DIR=$1
cmake --build $TESTS_DIR/build
ctest --test-dir $TESTS_DIR/build --output-on-failure