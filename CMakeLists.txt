cmake_minimum_required(VERSION 3.1)
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
include(FindOS)

# In-source builds are disabled.
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "CMAKE generation is not possible within the source directory!")
endif()

# Set VERSION
set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_MINOR_VERSION 19)
set(${PROJECT_NAME}_PATCH_VERSION 2)
set(${PROJECT_NAME}_VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION})

message(STATUS "Crytox release ${${PROJECT_NAME}_VERSION}")

# Options
option(${${PROJECT_NAME}_ID}_BUILD_BUTEST "Build Boost Unit Test" ON)

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
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE ON)

# Direct output to certain directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Project consists of multiple subprojects,
# include the directories
add_subdirectory(ui)
add_subdirectory(coilcl)
add_subdirectory(crycli)
add_subdirectory(pfbase)
add_subdirectory(pfupgr)
add_subdirectory(plgex)
add_subdirectory(primcore)
add_subdirectory(entropysrv)
#add_subdirectory(cmncore)
#add_subdirectory(shapework)
#add_subdirectory(shellasset)
add_subdirectory(crylib)

if (BUILD_BUTEST)
	message(STATUS "Building with Boost Unit Test")
	add_subdirectory(butest)
endif()
