#!/bin/bash
set -euf -o pipefail

BIN="$( cd  "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT=$( cd $BIN/../ && pwd )
BUILD_DIR=$ROOT/build

# Apply some cat sed grep trickery to extract
# APP_LIB_NAME and APP_EXE_NAME from CMake config.
APP_LIB_NAME=$(cat $ROOT/CMakeLists.txt \
                   | grep -m1 APP_LIB_NAME | sed 's/[a-z\s(].*APP_LIB_NAME\s\(.*\))/\1/')

EXE_LINK=$ROOT/bin/$APP_LIB_NAME
EXE_BUILD=$BUILD_DIR/src/$APP_LIB_NAME

