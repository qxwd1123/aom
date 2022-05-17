#!/usr/bin/bash

# export CFLAGS=-g
# export CXXFLAGS=-g

# if [[ ! -h /aom ]]; then
#     rm -rf /aom
#     ln $(pwd) /aom
# fi
# cd /aom
mkdir -p cmbuild
cd cmbuild
if [[ ! -a aomdec ]]; then
    cmake ../ \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCONFIG_AV1_ENCODER=0 \
    -DCONFIG_AV1_DECODER=1 \
    -DAOM_TARGET_CPU=generic \
    -DCONFIG_MULTITHREAD=0 \
    -DCONFIG_HW=1 \
    -DENABLE_CCACHE=1 \
    -DSANITIZE=address,undefined \
    -DENABLE_DOCS=0
fi
cmake --build ./
