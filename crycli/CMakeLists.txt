# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

project(Crycli CXX)

# Set project info
project_version(${PROJECT_NAME} 9)
project_description(${PROJECT_NAME} "Compiler CLI Interface")

# Load project defaults
include(ProjectPrep)

# Generate resource file
configure_file(${${PROJECT_NAME}_RESOURCE}/crycli.rc.in ${CMAKE_CURRENT_BINARY_DIR}/crycli.rc @ONLY)

include_directories(${CoilCl_INCLUDE_DIRS})

# Ignore security checks
enable_unsecure_crt()

add_executable(${PROJECT_NAME}
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
	${${PROJECT_NAME}_rel}
	${${PROJECT_NAME}_res}
)

# Set project options
include(ProjectFin)

# Define output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
	OUTPUT_NAME "crycli"
	PROJECT_LABEL "${Cryptox_ID} Compiler CLI Interface"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(${PROJECT_NAME}
	CoilCl
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_PROGRAM_OPTIONS_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_LIBRARIES}
)
