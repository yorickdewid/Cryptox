# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

# At least this version of CMake but newer is better
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(Cryptox)

# Define helper keywords
set(${PROJECT_NAME}_ID Cry)
set(${PROJECT_NAME}_CIL CIL)

if(POLICY CMP0025)
	cmake_policy(SET CMP0025 NEW)
endif()

# Include modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Find the host operating system and architecture
include(Util)
include(FindOS)
include(CheckCXXCompilerFlag)

# In-source builds are disabled.
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "CMAKE generation is not possible within the source directory!")
endif()

# Options
set(${${PROJECT_NAME}_ID}_RELEASE_ADD CACHE STRING "Version extension")
option(${${PROJECT_NAME}_ID}_RELEASE_CE "Build community release" OFF)
option(${${PROJECT_NAME}_ID}_BUILD_BUTEST "Build Boost Unit Test" ON)
option(${${PROJECT_NAME}_ID}_BUILD_LZ4XX "Build with LZ4" ON)
option(${${PROJECT_NAME}_ID}_BUILD_WALL "Build with all warning options" OFF)
set(${${PROJECT_NAME}_ID}_BRAND_NAME "Cryptox" CACHE STRING "Product name")
set(${${PROJECT_NAME}_ID}_BRAND_NAME_FULL "Cryptox Cryptography Suite" CACHE STRING "Product full description name")
set(${${PROJECT_NAME}_ID}_BRAND_AUTHOR "Blub Corp." CACHE STRING "Product author")

# Copyright
string(TIMESTAMP ${PROJECT_NAME}_YEAR "%Y")
if (${${${PROJECT_NAME}_ID}_RELEASE_CE})
	set(${PROJECT_NAME}_COPYRIGHT "Copyright (C) ${${PROJECT_NAME}_YEAR} Community Edition.")
else()
	set(${PROJECT_NAME}_COPYRIGHT "Copyright (C) ${${PROJECT_NAME}_YEAR} ${${${PROJECT_NAME}_ID}_BRAND_AUTHOR} All rights reserved.")
endif()

# Set version
set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 19)
set(${PROJECT_NAME}_VERSION_PATCH 2)
set(${PROJECT_NAME}_VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH})

if (NOT ${${${PROJECT_NAME}_ID}_RELEASE_ADD} STREQUAL "")
	set(${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION}-${${${PROJECT_NAME}_ID}_RELEASE_ADD}")
endif()

# Static config exports
string(TIMESTAMP ${PROJECT_NAME}_COMPILE_TIMESTAMP "%Y-%m-%d %H:%M:%S")
set(PRODUCT_VERSION_MAJOR ${${PROJECT_NAME}_VERSION_MAJOR})
set(PRODUCT_VERSION_MINOR ${${PROJECT_NAME}_VERSION_MINOR})
set(PRODUCT_VERSION_PATCH ${${PROJECT_NAME}_VERSION_PATCH})
set(PRODUCT_VERSION ${${PROJECT_NAME}_VERSION})
set(PRODUCT_COMPILE_TIMESTAMP ${${PROJECT_NAME}_COMPILE_TIMESTAMP})
set(PRODUCT_BRAND_NAME ${${${PROJECT_NAME}_ID}_BRAND_NAME})
set(PRODUCT_BRAND_NAME_FULL ${${${PROJECT_NAME}_ID}_BRAND_NAME_FULL})
set(PRODUCT_BRAND_AUTHOR ${${${PROJECT_NAME}_ID}_BRAND_AUTHOR})
set(PRODUCT_COPYRIGHT ${${PROJECT_NAME}_COPYRIGHT})

if (NOT ${${${PROJECT_NAME}_ID}_RELEASE_ADD} STREQUAL "")
	set(PRODUCT_VERSION_EXT ${${${PROJECT_NAME}_ID}_RELEASE_ADD})
endif()

if (${${${PROJECT_NAME}_ID}_RELEASE_CE})
	set(PRODUCT_IS_CE 1)
else()
	set(PRODUCT_IS_CE 0)
endif()

message(STATUS "${${${PROJECT_NAME}_ID}_BRAND_NAME_FULL} rel ${${PROJECT_NAME}_VERSION}")
message(STATUS ${${PROJECT_NAME}_COPYRIGHT})

# Use solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets")

if (WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

# Boost options
if(WIN32)
	set(Boost_USE_STATIC_LIBS ON)
endif()

# Locate boost
find_package(Boost 1.61 REQUIRED COMPONENTS
	program_options
	filesystem
	unit_test_framework
)

# Add boost to include
if(Boost_FOUND)
	message(STATUS "Using boost version ${Boost_VERSION}")
	include_directories(${Boost_INCLUDE_DIRS})
	add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
endif()

# Check wxWidgets
#find_package(wxWidgets COMPONENTS)

#if(wxWidgets_FOUND)
#	include(${wxWidgets_USE_FILE})
#endif()

# Compiler flags
if(COMPILER_SUPPORTS_CXX17)
	set(CMAKE_CXX_STANDARD 17)
	message(STATUS "Using C++17")
else()
	set(CMAKE_CXX_STANDARD 14)
	message(STATUS "Using C++14")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build sources with additional warnings
if(${${PROJECT_NAME}_ID}_BUILD_WALL)
	if(CMAKE_COMPILER_IS_GNUCC)
		set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
	endif()
	if(MSVC)
		set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4 /sdl")
	endif()
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE ON)

# Direct output to certain directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Project consists of multiple subprojects
add_subdirectory(cmncore)
add_subdirectory(coilcex)
add_subdirectory(coilcl)
add_subdirectory(crycli)
add_subdirectory(cexdump)
add_subdirectory(lddriver)
add_subdirectory(pfbase)
add_subdirectory(pfupgr)
add_subdirectory(plgex)
#add_subdirectory(primcore)
#add_subdirectory(entropysrv)
#add_subdirectory(crylib)

# UI parts are disabled
#add_subdirectory(shapework)
#add_subdirectory(shellasset)
#add_subdirectory(ui)

if (${${PROJECT_NAME}_ID}_BUILD_BUTEST)
	message(STATUS "Building with Boost Unit Test")
	add_subdirectory(butest)
endif()

if (${${PROJECT_NAME}_ID}_BUILD_LZ4XX)
	add_subdirectory(3rdparty/lz4xx)
endif()
