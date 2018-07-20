# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

# Create alternative names
string(TOLOWER ${PROJECT_NAME} PRODUCT_ORIGINAL_NAME)
string(TOUPPER ${PROJECT_NAME} PRODUCT_FORMAL_NAME)
set(PRODUCT_VERSION_LOCAL ${${PROJECT_NAME}_VERSION_LOCAL})
set(PRODUCT_DESCRIPTION ${${PROJECT_NAME}_DESCRIPTION})

# Generated CMake files before target
configure_file(${COMMON_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/Cry/Config.h @ONLY)
configure_file(${COMMON_DIR}/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/res/version.rc @ONLY)

# Project directories
set(${PROJECT_NAME}_SRC src)
set(${PROJECT_NAME}_INCLUDE include)

# On Windows include resource files
if(WIN32)
	set(${PROJECT_NAME}_RESOURCE res)
endif()

# Fetch sources
file(GLOB ${PROJECT_NAME}_src  ${${PROJECT_NAME}_SRC}/*.cpp ${${PROJECT_NAME}_SRC}/*.c ${${PROJECT_NAME}_SRC}/*.h)
file(GLOB_RECURSE ${PROJECT_NAME}_h ${${PROJECT_NAME}_INCLUDE}/*.h)
file(GLOB ${PROJECT_NAME}_rel ${CMAKE_CURRENT_SOURCE_DIR}/*.rc ${CMAKE_CURRENT_SOURCE_DIR}/*.txt ${CMAKE_CURRENT_SOURCE_DIR}/*.man ${CMAKE_CURRENT_SOURCE_DIR}/*.in)

include_directories(${${PROJECT_NAME}_INCLUDE})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(${GEN_SOURCE_DIR})

# Include common headers in every project
include_directories(${CommonCore_INCLUDE_DIRS})

# On Windows include self generated resource files
if(WIN32)
	file(GLOB ${PROJECT_NAME}_res ${CMAKE_CURRENT_BINARY_DIR}/res/*.rc)
endif()

message(STATUS "Configure ${PROJECT_NAME} rel. ${PRODUCT_VERSION_LOCAL}")
