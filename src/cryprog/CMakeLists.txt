# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

# Set project info
project(CryProg CXX)
project_version(${PROJECT_NAME} 2)
project_description(${PROJECT_NAME} "Program Library")

# Load project defaults
include(ProjectPrep)

# External includes
include_directories(${CryCC_INCLUDE_DIRS})
include_directories(${CoilCl_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
	${${PROJECT_NAME}_rel}
	${${PROJECT_NAME}_res}
)

# Define output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
	OUTPUT_NAME "cprg"
	PROJECT_LABEL "${Cryptox_ID} Program Library"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(${PROJECT_NAME}
	CryCC
	${Boost_LIBRARIES}
)

# Enable tests on this target
enable_auto_test("${Cryptox_ID} Program Library Test")

# Set project options
include(ProjectFin)
