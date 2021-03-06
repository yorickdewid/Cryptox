# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

# Set project info
project(ShellAsset CXX)
project_version(${PROJECT_NAME} 1)
project_description(${PROJECT_NAME} "Cry Compiler Shell")

# Load project defaults
include(ProjectPrep)

# Ignore security checks
enable_unsecure_crt()

# External includes
include_directories(${CryCC_INCLUDE_DIRS})
include_directories(${CoilCl_INCLUDE_DIRS})
include_directories(${CryEVM_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
	${${PROJECT_NAME}_rel}
	${${PROJECT_NAME}_res}
)

# Define output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
	OUTPUT_NAME "crysh"
	PROJECT_LABEL "${Cryptox_ID} Compiler Shell"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(${PROJECT_NAME}
	CryCC
	CoilCl
	CryEVM
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_PROGRAM_OPTIONS_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_LIBRARIES}
)

# Set project options
include(ProjectFin)
