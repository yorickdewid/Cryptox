# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

# Set project info
project(BUTest CXX)
project_version(${PROJECT_NAME} 2)
project_description(${PROJECT_NAME} "Unit Testing")

# Load project defaults
include(ProjectPrep)

# External includes
include_directories(${PfBase_INCLUDE_DIRS})
include_directories(${CoilCEX_INCLUDE_DIRS})
include_directories(${CoilCl_INCLUDE_DIRS})
include_directories(${CryEVM_INCLUDE_DIRS})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Ignore security checks
enable_unsecure_crt()

# Allow testing on this target
enable_testing()

add_executable(${PROJECT_NAME}
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
	OUTPUT_NAME "boost-test"
	PROJECT_LABEL "Boost Unit Test"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if (UNIX)
	add_definitions(-DBOOST_TEST_DYN_LINK) 
endif()

target_link_libraries(${PROJECT_NAME}
	PfBase
	CoilCEX
	CoilCl
	CryEVM
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	${Boost_LIBRARIES}
)

# Add test
add_test(NAME ${PROJECT_NAME}
	COMMAND ${CMAKE_BINARY_DIR}/bin/boost-test
)

# Set project options
include(ProjectFin)
