# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be
# copied and/or distributed without the express of the author.

# At least this version of CMake but newer is better
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(Cryptox
    VERSION 0.9.2.0
	DESCRIPTION "Cryptographic Toolkit"
)

message(STATUS "Compiler ${CMAKE_CXX_COMPILER_ID} version ${CMAKE_CXX_COMPILER_VERSION}")
if(MSVC)
	message(STATUS "Platform Toolset ${CMAKE_VS_PLATFORM_TOOLSET}")
	message(STATUS "Windows SDK ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
endif()

# TODO: DELETE
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

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
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
include(AutoTest)
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
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
        message(FATAL_ERROR "Compiler does not support C++17")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Require at least Clang 3.3
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4)
        message(FATAL_ERROR "Compiler does not support C++17")
	endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	# Require at least MSVC 14.0
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14.0)
        message(FATAL_ERROR "Compiler does not support C++17")
	endif()
else()
    message(WARNING "Unknown compiler ${CMAKE_CXX_COMPILER_ID}")
endif()

# Build options
option(${${PROJECT_NAME}_ID}_RELEASE_CE "Build community release" OFF)
option(${${PROJECT_NAME}_ID}_BUILD_UNITTEST "Build Boost Unit Test" ON)
option(${${PROJECT_NAME}_ID}_BUILD_LZ4XX "Build with LZ4" ON)
option(${${PROJECT_NAME}_ID}_BUILD_MSGGEN "Build event message generator" ON)
option(${${PROJECT_NAME}_ID}_BUILD_QUID "Build QUID identifier library" ON)
option(${${PROJECT_NAME}_ID}_BUILD_WALL "Build with all warning options" OFF)

# -------------------------------
# Product characterstics
# -------------------------------

# Get build number, and set as tweak number
if($ENV{APPVEYOR_BUILD_NUMBER})
	set(PROJECT_VERSION_BUILD $ENV{APPVEYOR_BUILD_NUMBER})
	set(PRODUCT_VERSION_TWEAK ${PROJECT_VERSION_BUILD})
endif()

# Get source control reference
execute_process(
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMAND ${GIT_EXE} name-rev HEAD --tags --name-only --no-undefined --always
	OUTPUT_VARIABLE PROJECT_VERSION_REV
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get compile timestamp
string(TIMESTAMP PROJECT_COMPILE_TIMESTAMP "%Y-%m-%d %H:%M:%S")

# Initialize product vars
set(PRODUCT_BRAND_NAME ${PROJECT_NAME} CACHE STRING "Product name")
set(PRODUCT_BRAND_NAME_FULL ${PROJECT_DESCRIPTION} CACHE STRING "Product description")
set(PRODUCT_BRAND_AUTHOR "Blub Corp." CACHE STRING "Product author")
set(PRODUCT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(PRODUCT_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(PRODUCT_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(PRODUCT_VERSION_TWEAK ${PROJECT_VERSION_TWEAK})
set(PRODUCT_VERSION ${PROJECT_VERSION})
set(PRODUCT_COMPILE_TIMESTAMP ${PROJECT_COMPILE_TIMESTAMP})
set(PRODUCT_COPYRIGHT ${${PROJECT_NAME}_COPYRIGHT})
set(PRODUCT_VERSION_BUILD ${PROJECT_VERSION_BUILD})
set(PRODUCT_VERSION_REV ${PROJECT_VERSION_REV})
set(PRODUCT_VERSION_NAME CACHE STRING "Product version name")

if (${${${PROJECT_NAME}_ID}_RELEASE_CE})
	set(PRODUCT_IS_CE 1)
else()
	set(PRODUCT_IS_CE 0)
endif()

# Get copyright string
string(TIMESTAMP PROJECT_COMPILE_YEAR "%Y")
if (${${${PROJECT_NAME}_ID}_RELEASE_CE})
	set(PRODUCT_COPYRIGHT "Copyright (C) ${PROJECT_COMPILE_YEAR} Community Edition." CACHE STRING "Product copyright message")
else()
	set(PRODUCT_COPYRIGHT "Copyright (C) ${PROJECT_COMPILE_YEAR} ${PRODUCT_BRAND_AUTHOR} All rights reserved." CACHE STRING "Product copyright message")
endif()

message(STATUS "\n== PRODUCT ==")
message(STATUS "Name\t${PRODUCT_BRAND_NAME}")
message(STATUS "Descr.\t${PRODUCT_BRAND_NAME_FULL}")
message(STATUS "Author\t${PRODUCT_BRAND_AUTHOR}")
message(STATUS "Copyright\t${PRODUCT_COPYRIGHT}")
message(STATUS "Version\t${PRODUCT_VERSION}")
message(STATUS "Ver. Name\t${PRODUCT_VERSION_NAME}")
message(STATUS "Timestamp\t${PRODUCT_COMPILE_TIMESTAMP}")
message(STATUS "Build No.\t${PRODUCT_VERSION_BUILD}")
message(STATUS "Revision\t${PRODUCT_VERSION_REV}")
message(STATUS "CE Release\t${PRODUCT_IS_CE}")
message(STATUS "== /PRODUCT ==\n")

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
message(STATUS "Using ISO C++17")
set(CMAKE_CXX_STANDARD 17)
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
file(MAKE_DIRECTORY ${GEN_SOURCE_DIR}/Cry)
execute_process(
	COMMAND ${MSGGEN_EXE} ${COMMON_DIR}/manifest.xml ${GEN_SOURCE_DIR}/Cry/EventLog.h
)

# -------------------------------
# Include subprojects
# -------------------------------

# Project consists of multiple subprojects
add_subdirectory(src/crywork)
add_subdirectory(src/crycc)
add_subdirectory(src/coilcex)
add_subdirectory(src/coilcl)
add_subdirectory(src/cryprog)
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
add_subdirectory(src/primcore)

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
