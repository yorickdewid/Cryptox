# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

cmake_minimum_required(VERSION 2.8)

# Set project info
project(lz4xx CXX)

include_directories(include)

file(GLOB_RECURSE SRC src/*.cpp)
file(GLOB_RECURSE INCLUDES include/*.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")

add_library(lz4xx ${SRC})

# Define output directories
set_target_properties(lz4xx
	PROPERTIES
	OUTPUT_NAME "lz4xx"
	PROJECT_LABEL "LZ4 C++"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
