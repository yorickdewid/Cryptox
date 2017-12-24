# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

if (NOT ${${PROJECT_NAME}_VERSION_LOCAL})
	message(FATAL_ERROR "Set local version")
endif()

if (${${PROJECT_NAME}_DESCRIPTION} STREQUAL "")
	message(FATAL_ERROR "Set project description")
endif()

string(TOLOWER ${PROJECT_NAME} PRODUCT_ORIGINAL_NAME)
set(PRODUCT_VERSION_LOCAL ${${PROJECT_NAME}_VERSION_LOCAL})
set(PRODUCT_DESCRIPTION ${${PROJECT_NAME}_DESCRIPTION})

# Project directories
set(${PROJECT_NAME}_SRC src)
set(${PROJECT_NAME}_INCLUDE include)
set(${PROJECT_NAME}_RESOURCE res)

# Fetch sources
file(GLOB ${PROJECT_NAME}_src ${${PROJECT_NAME}_SRC}/*.cpp ${${PROJECT_NAME}_SRC}/*.h)
file(GLOB ${PROJECT_NAME}_h ${${PROJECT_NAME}_INCLUDE}/*.h)
file(GLOB ${PROJECT_NAME}_rel ${${PROJECT_NAME}_RESOURCE}/*.in ${${PROJECT_NAME}_RESOURCE}/*.rc ${${PROJECT_NAME}_RESOURCE}/*.txt)

if(WIN32)
	file(GLOB ${PROJECT_NAME}_res ${CMAKE_CURRENT_BINARY_DIR}/*.rc)
endif()

include_directories(${${PROJECT_NAME}_INCLUDE})

message(STATUS "Configure ${PROJECT_NAME} rel. ${PRODUCT_VERSION_LOCAL}")
