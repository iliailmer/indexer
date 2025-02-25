#!/bin/bash

BUILD_DIR="build"

if [ "$1" == "clean" ]; then
  rm -rf "$BUILD_DIR"
  echo "Cleaned build directory."
  exit 0
fi
if [ ! -d "$BUILD_DIR" ]; then
  meson setup "$BUILD_DIR"
else
  meson setup --reconfigure "$BUILD_DIR"
fi

ninja -C "$BUILD_DIR"

./"$BUILD_DIR"/indexer $1
