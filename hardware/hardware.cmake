#
# SPDX-License-Identifier: (GPL-2.0+ OR MIT)
#
# Copyright (c) 2022 Rockchip Electronics Co., Ltd
# 

if(NOT CONFIG_HW)
    return()
endif()

list(APPEND HARDWARE_SOURCES
    "${AOM_ROOT}/hardware/hw_global.h"
    "${AOM_ROOT}/hardware/hw_global.c"
    "${AOM_ROOT}/hardware/hw_utils.h"
    "${AOM_ROOT}/hardware/hw_utils.c"
    "${AOM_ROOT}/hardware/hw_filter_stat.h"
    "${AOM_ROOT}/hardware/hw_filter_stat.c"
    )

# Creates hardware build targets. Must not be called until after libaom target
# has been created.
function(setup_hardware_targets)
    add_library(hardware OBJECT ${HARDWARE_SOURCES})
    target_sources(aom PRIVATE $<TARGET_OBJECTS:hardware>)
    if(BUILD_SHARED_LIBS)
        target_sources(aom_static PRIVATE $<TARGET_OBJECTS:hardware>)
    endif()
    list(APPEND AOM_LIB_TARGETS hardware)

    # Pass the new lib targets up to the parent scope instance of
    # $AOM_LIB_TARGETS.
    set(AOM_LIB_TARGETS ${AOM_LIB_TARGETS} PARENT_SCOPE)
endfunction()
