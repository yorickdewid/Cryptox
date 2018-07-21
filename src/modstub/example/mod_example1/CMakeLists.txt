# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

# Set project info
project(ModStubEx1 CXX)
project_version(${PROJECT_NAME} 2)
project_description(${PROJECT_NAME} "External module example1")

# Load project defaults
include(ProjectPrep)

# External includes
include_directories(${ModStub_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED
	mod_example1.cpp
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
	${${PROJECT_NAME}_rel}
	${${PROJECT_NAME}_res}
)

# Define output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
	OUTPUT_NAME "mod_example1"
	PROJECT_LABEL "Module Example1"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(${PROJECT_NAME}
	ModStub
)

# Set project options
include(ProjectFin)
