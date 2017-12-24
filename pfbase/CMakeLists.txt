# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

project(PfBase CXX)

# Set project info
project_version(${PROJECT_NAME} 1)
project_description(${PROJECT_NAME} "Project File Base")

# Load project defaults
include(ProjectPrep)

# Ignore security checks
enable_unsecure_crt()

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
	OUTPUT_NAME "pfbase"
	PROJECT_LABEL "Project File Base"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Link static libraries
target_link_libraries(${PROJECT_NAME}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_LIBRARIES}
)
