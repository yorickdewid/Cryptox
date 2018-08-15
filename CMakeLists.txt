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

if(POLICY CMP0026)
  cmake_policy(SET CMP0026 NEW)
endif()

if(POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()

if(POLICY CMP0046)
  cmake_policy(SET CMP0046 NEW)
endif()

if(POLICY CMP0051)
  cmake_policy(SET CMP0051 NEW)
endif()

if(POLICY CMP0056)
  cmake_policy(SET CMP0056 NEW)
endif()

if(POLICY CMP0067)
  cmake_policy(SET CMP0067 NEW)
endif()

# Include modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

# Find the host operating system and architecture
include(Util)
include(CPack)
include(CTest)
include(FindOS)
include(FindGit)
include(CheckCXXCompilerFlag)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

# Force build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type for Swift [Debug, RelWithDebInfo, Release, MinSizeRel]" FORCE)
	message(STATUS "No build type was specified, will default to ${CMAKE_BUILD_TYPE}")
endif()

# In-source builds are disabled.
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "CMAKE generation is not possible within the source directory!")
endif()

# Compiler check
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Require at least GCC 4.8
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "Compiler does not support C++11")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Require at least Clang 3.3
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3)
        message(FATAL_ERROR "Compiler does not support C++11")
	endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	# Require at least MSVC 14.0
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14.0)
        message(FATAL_ERROR "Compiler does not support C++11")
	endif()
else()
    message(WARNING "Unknown compiler")
endif()

# Options
set(${${PROJECT_NAME}_ID}_RELEASE_ADD CACHE STRING "Version extension")
option(${${PROJECT_NAME}_ID}_RELEASE_CE "Build community release" OFF)
option(${${PROJECT_NAME}_ID}_BUILD_UNITTEST "Build Boost Unit Test" ON)
option(${${PROJECT_NAME}_ID}_BUILD_LZ4XX "Build with LZ4" ON)
option(${${PROJECT_NAME}_ID}_BUILD_MSGGEN "Build event message generator" ON)
option(${${PROJECT_NAME}_ID}_BUILD_QUID "Build QUID identifier library" ON)
option(${${PROJECT_NAME}_ID}_BUILD_WALL "Build with all warning options" OFF)
set(${${PROJECT_NAME}_ID}_BRAND_NAME "Cryptox" CACHE STRING "Product name")
set(${${PROJECT_NAME}_ID}_BRAND_NAME_FULL "Cryptox Cryptography Suite" CACHE STRING "Product full description name")
set(${${PROJECT_NAME}_ID}_BRAND_AUTHOR "Blub Corp." CACHE STRING "Product vendor")

# Copyright
string(TIMESTAMP ${PROJECT_NAME}_YEAR "%Y")
if (${${${PROJECT_NAME}_ID}_RELEASE_CE})
	set(${PROJECT_NAME}_COPYRIGHT "Copyright (C) ${${PROJECT_NAME}_YEAR} Community Edition.")
else()
	set(${PROJECT_NAME}_COPYRIGHT "Copyright (C) ${${PROJECT_NAME}_YEAR} ${${${PROJECT_NAME}_ID}_BRAND_AUTHOR} All rights reserved.")
endif()

# -------------------------------
# Project version control
# -------------------------------

# Set version
set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 19)
set(${PROJECT_NAME}_VERSION_PATCH 2)
set(${PROJECT_NAME}_VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH})

if (NOT ${${${PROJECT_NAME}_ID}_RELEASE_ADD} STREQUAL "")
	set(${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION}-${${${PROJECT_NAME}_ID}_RELEASE_ADD}")
endif()

if($ENV{APPVEYOR_BUILD_NUMBER})
	set(${PROJECT_NAME}_VERSION_BUILD $ENV{APPVEYOR_BUILD_NUMBER})
	message(STATUS "Build nr ${${PROJECT_NAME}_VERSION_BUILD}")
endif()

# Get source control reference
execute_process(
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMAND ${GIT_EXE} name-rev HEAD --tags --name-only --no-undefined --always
	OUTPUT_VARIABLE ${PROJECT_NAME}_VERSION_REV
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Build rev ${${PROJECT_NAME}_VERSION_REV}")

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

if(${${PROJECT_NAME}_VERSION_BUILD})
	set(PRODUCT_VERSION_BUILD ${PROJECT_NAME}_VERSION_BUILD)
endif()

if(${${PROJECT_NAME}_VERSION_REV})
	set(PRODUCT_VERSION_REV ${PROJECT_NAME}_VERSION_REV)
endif()

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

# -------------------------------
# Installation package settings
# -------------------------------

# Set package details
set(CPACK_PACKAGE_NAME ${PRODUCT_BRAND_NAME})
set(CPACK_PACKAGE_VENDOR ${PRODUCT_BRAND_AUTHOR})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PRODUCT_BRAND_NAME_FULL})
set(CPACK_PACKAGE_VERSION ${PRODUCT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PRODUCT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PRODUCT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PRODUCT_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PRODUCT_BRAND_NAME})
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY ${PRODUCT_BRAND_NAME})
SET(CPACK_PACKAGE_FILE_NAME "${PRODUCT_BRAND_NAME}-${${PROJECT_NAME}_VERSION}")
SET(CPACK_NSIS_DISPLAY_NAME "${${${PROJECT_NAME}_ID}_BRAND_NAME_FULL} ${${PROJECT_NAME}_VERSION}")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
SET(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

# Use solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets")

# -------------------------------
# External package configuration
# -------------------------------

# Boost options
if(WIN32)
	set(Boost_USE_STATIC_LIBS ON)
endif()

# Locate boost
find_package(Boost 1.57 REQUIRED COMPONENTS
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

# -------------------------------
# Compiler options
# -------------------------------

# Make settings
#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

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
	if(MSVC)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4")
	elseif(MINGW)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Werror -Wno-error=format= -Wno-error=format -Wno-error=format-extra-args")
	else()
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror -Wall")
	endif()
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /sdl")
endif()

# -------------------------------
# Location settings
# -------------------------------

# Direct output to certain directories
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib/Debug")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib/Release")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib/Debug")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib/Release")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release")

# Top level source directories
set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)
set(CONTRIB_DIR ${CMAKE_SOURCE_DIR}/contrib)
set(3RD_PROJECT_DIR ${CMAKE_SOURCE_DIR}/third_party)
set(GEN_SOURCE_DIR ${CMAKE_BINARY_DIR}/include)

# Fing Message Event Generator
include(FindMsgGen)

# Generate the event logging header
file(MAKE_DIRECTORY ${GEN_SOURCE_DIR})
execute_process(
	COMMAND ${MSGGEN_EXE} ${COMMON_DIR}/manifest.xml ${GEN_SOURCE_DIR}/EventLog.h
)

# -------------------------------
# Include subprojects
# -------------------------------

# Project consists of multiple subprojects
add_subdirectory(src/crywork)
add_subdirectory(src/crycc)
add_subdirectory(src/cryprog)
add_subdirectory(src/coilcex)
add_subdirectory(src/coilcl)
add_subdirectory(src/cryevm)
add_subdirectory(src/crycli)
add_subdirectory(src/cexdump)
add_subdirectory(src/shellasset)
add_subdirectory(src/lddriver)
add_subdirectory(src/pfbase)
add_subdirectory(src/pfupgr)
add_subdirectory(src/modstub)
add_subdirectory(src/entropysrv)
add_subdirectory(src/crylib)
add_subdirectory(src/quidex)
#add_subdirectory(src/primcore)

# UI parts are disabled
#add_subdirectory(src/shapework)
#add_subdirectory(src/ui)

if (${${PROJECT_NAME}_ID}_BUILD_LZ4XX)
	add_subdirectory(third_party/lz4xx)
endif()

if (${${PROJECT_NAME}_ID}_BUILD_MSGGEN)
	add_subdirectory(third_party/msggen)
endif()


if (${${PROJECT_NAME}_ID}_BUILD_QUID)
	add_subdirectory(third_party/QUID)
endif()

if (${${PROJECT_NAME}_ID}_BUILD_UNITTEST)
	message(STATUS "Building with Unit Test")
	add_subdirectory(test)
endif()

install(DIRECTORY DESTINATION "redist")
