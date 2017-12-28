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

# Allow testing on this target
enable_testing()

# Add it to test execution TODO
add_test("Cryptox Test" "boost-test")

add_executable(${PROJECT_NAME}
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
	${${PROJECT_NAME}_rel}
	${${PROJECT_NAME}_res}
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

target_link_libraries(${PROJECT_NAME}
	PfBase
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	${Boost_LIBRARIES}
)

# Set project options
include(ProjectFin)
