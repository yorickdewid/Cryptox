# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

project(quidex CXX)
project_version(${PROJECT_NAME} 1)
project_description(${PROJECT_NAME} "CIL QUID Extension")

# Load project defaults
include(ProjectPrep)

# Ignore security checks
#enable_unsecure_crt()

# External includes
include_directories(${CryEVM_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
	${${PROJECT_NAME}_rel}
    ${${PROJECT_NAME}_res}
	"../coilcl/src/Valuedef.cpp"
	"../coilcl/src/Typedef.cpp"
	"../coilcl/src/TypeFacade.cpp"
	"../coilcl/src/RecordValue.cpp"
)

# Define output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
	OUTPUT_NAME "quidex"
	PROJECT_LABEL "CIL QUID Extension"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Set project options
include(ProjectFin)
