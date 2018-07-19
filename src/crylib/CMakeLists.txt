# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

project(Crylib CXX)
project_version(${PROJECT_NAME} 0)
project_description(${PROJECT_NAME} "CIL Standard Library")

# Load project defaults
include(ProjectPrep)

# Copy headers to binary directory
file(GLOB files ${${PROJECT_NAME}_INCLUDE}/*.in)
foreach(file ${files})
  get_filename_component(file_source ${file} NAME)
  string(REGEX REPLACE "\\.[^.]*$" "" file_source_we ${file_source})
  configure_file(${file} ${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}/${file_source_we} @ONLY)
endforeach()

# Ignore security checks
enable_unsecure_crt()

add_library(${PROJECT_NAME} SHARED
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
	${${PROJECT_NAME}_rel}
	${${PROJECT_NAME}_res}
)

# Define output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
	OUTPUT_NAME "libcil"
	PROJECT_LABEL "CIL Standard Library"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Set project options
include(ProjectFin)
