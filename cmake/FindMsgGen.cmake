# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

find_program(MSGGEN_EXE msggen
    PATHS
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/*
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*
)
# TODO: select x64 or x32 based on arch
if(NOT MSGGEN_EXE)
    find_program(MSGGEN_EXE msggen
        NAMES msggen_x64
        PATHS ${CONTRIB_DIR}
    )
    if(NOT MSGGEN_EXE)
        message(FATAL_ERROR "Generator not found")
    endif()
    message(STATUS "Using contrib event generator")
endif()
