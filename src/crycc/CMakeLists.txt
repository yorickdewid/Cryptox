# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

# Set project info
project(CryCC CXX)
project_version(${PROJECT_NAME} 3)
project_description(${PROJECT_NAME} "Compiler Core")

# Load project defaults
include(ProjectPrep)

# External includes
include_directories(${LZ4xx_INCLUDE_DIRS})

add_library(${PROJECT_NAME} STATIC
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
	${${PROJECT_NAME}_rel}
	${${PROJECT_NAME}_res}
)

# Define output directories
set_target_properties(${PROJECT_NAME}
	PROPERTIES
	OUTPUT_NAME "crycc"
	PROJECT_LABEL "${Cryptox_ID} Compiler Core"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Enable tests on this target
enable_auto_test("${Cryptox_ID} Compiler Core Test")

# Set project options
include(ProjectFin)
