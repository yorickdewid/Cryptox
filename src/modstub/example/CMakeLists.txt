# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

# Set project info
project(ModStubExamp CXX)
project_version(${PROJECT_NAME} 2)
project_description(${PROJECT_NAME} "External module examples")

# Load project defaults
include(ProjectPrep)

# Ignore security checks
# enable_unsecure_crt()

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
	PROJECT_LABEL "Module Example"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(${PROJECT_NAME}
	ModStub
)

# target_link_libraries(${PROJECT_NAME}
# 	${Boost_FILESYSTEM_LIBRARY}
# 	${Boost_PROGRAM_OPTIONS_LIBRARY}
# 	${Boost_SYSTEM_LIBRARY}
# 	${Boost_LIBRARIES}
# )

# Set project options
include(ProjectFin)
