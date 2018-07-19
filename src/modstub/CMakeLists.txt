# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

# Set project info
project(ModStub CXX)
project_version(${PROJECT_NAME} 7)
project_description(${PROJECT_NAME} "Module Stub")

# Load project defaults
include(ProjectPrep)

add_library(${PROJECT_NAME} STATIC
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
	${${PROJECT_NAME}_rel}
	${${PROJECT_NAME}_res}
)

# Define output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
	OUTPUT_NAME "mdstb"
	PROJECT_LABEL "Module Stub"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Set project options
include(ProjectFin)

option(${Cryptox_ID}_MOD_BUILD_EXAMPLE "Build external module examples" OFF)

if(${Cryptox_ID}_CIL_BUILD_EXAMPLE)
	add_subdirectory(example)
endif()
