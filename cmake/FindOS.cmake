# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

# CMake does not distinguish Linux from other Unices.
string(REGEX MATCH "Linux" PROJECT_OS_LINUX ${CMAKE_SYSTEM_NAME})
# Nor *BSD
string(REGEX MATCH "BSD" PROJECT_OS_BSD ${CMAKE_SYSTEM_NAME})
# Or Solaris. I'm seeing a trend, here
string(REGEX MATCH "SunOS" PROJECT_OS_SOLARIS ${CMAKE_SYSTEM_NAME})

# Windows is easy (for once)
if(WIN32)
    set(PROJECT_OS_WIN TRUE BOOL INTERNAL)
endif(WIN32)

# Check if it's an Apple OS
if(APPLE)
    # Check if it's OS X or another MacOS (that's got to be pretty unlikely)
    string(REGEX MATCH "Darwin" PROJECT_OS_OSX ${CMAKE_SYSTEM_NAME})
	if(NOT PROJECT_OS_OSX)
        set(PROJECT_OS_MACOS TRUE BOOL INTERNAL)
    endif(NOT PROJECT_OS_OSX)
endif(APPLE)

# QNX
if(QNXNTO)
    set(PROJECT_OS_QNX TRUE BOOL INTERNAL)
endif(QNXNTO)

if(PROJECT_OS_LINUX)
    message(STATUS "Operating system is Linux")
elseif(PROJECT_OS_BSD)
    message(STATUS "Operating system is BSD")
elseif(PROJECT_OS_WIN)
    message(STATUS "Operating system is Windows")
elseif(PROJECT_OS_OSX)
    message(STATUS "Operating system is Apple MacOS X")
elseif(PROJECT_OS_MACOS)
    message(STATUS "Operating system is Apple MacOS (not OS X)")
elseif(PROJECT_OS_QNX)
    message(STATUS "Operating system is QNX")
elseif(PROJECT_OS_SOLARIS)
    message(STATUS "Operating system is Solaris")
ELSE (PROJECT_OS_LINUX)
    message(STATUS "Operating system is generic Unix")
endif(PROJECT_OS_LINUX)

message(STATUS "System Processor ${CMAKE_SYSTEM_PROCESSOR}")

# 32 or 64 bit Linux
if(PROJECT_OS_LINUX)
    # Set the library directory suffix accordingly
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(PROJECT_PROC_64BIT TRUE BOOL INTERNAL)
        message(STATUS "Linux x86_64 Detected")
    elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "ppc64")
        message(STATUS "Linux ppc64 Detected")
        set(PROJECT_PROC_64BIT TRUE BOOL INTERNAL)
    endif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
endif(PROJECT_OS_LINUX)
