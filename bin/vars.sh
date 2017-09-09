#!/bin/bash
set -euf -o pipefail

BIN="$( cd  "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT=$( cd $BIN/../ && pwd )
BUILD_DIR=$ROOT/build

APP_LIB_NAME=ubelt
APP_MAIN_EXE=ubelt

EXE_NAME=$APP_MAIN_EXE
EXE_LINK=$ROOT/bin/$EXE_NAME
EXE_BUILD=$BUILD_DIR/src/$EXE_NAME

