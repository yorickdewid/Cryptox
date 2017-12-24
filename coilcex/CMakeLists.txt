# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

project(CoilCEX CXX)

# Set project info
project_version(${PROJECT_NAME} 3)
project_description(${PROJECT_NAME} "Executable Format")

# Load project defaults
include(ProjectPrep)

add_library(${PROJECT_NAME} SHARED
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
	OUTPUT_NAME "coilcex"
	PROJECT_LABEL "${Cryptox_ID} Executable Format"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(${PROJECT_NAME}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_LIBRARIES}
)
