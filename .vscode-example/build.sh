#!/usr/bin/bash

if [[ ! -d streams ]]; then
    ln -sf ../streams ./
fi

mkdir -p build_dir
cd build_dir
if [[ ! -a aomdec ]]; then
    cmake ../ \
    -GNinja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCONFIG_AV1_ENCODER=0 \
    -DCONFIG_AV1_DECODER=1 \
    -DAOM_TARGET_CPU=generic \
    -DCONFIG_MULTITHREAD=0 \
    -DENABLE_DOCS=0 \
    -DENABLE_CCACHE=1 \
    -DCONFIG_HW=1 \
    -DSANITIZE=address,undefined \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_C_FLAGS=-fcolor-diagnostics \
    -DCMAKE_CXX_FLAGS=-fcolor-diagnostics
fi
cmake --build ./
