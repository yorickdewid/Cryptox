# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

# Set project info
project(CommonCore CXX)
project_version(${PROJECT_NAME} 1)
project_description(${PROJECT_NAME} "Common Core Headers")

# Load project defaults
include(ProjectPrep)

add_custom_target(${PROJECT_NAME}
	SOURCES ${${PROJECT_NAME}_h}
)

# Public header
source_group("External Header Files" FILES ${${PROJECT_NAME}_h})

# Expose public includes to other
# subprojects through cache variable
set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${${PROJECT_NAME}_INCLUDE}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

# File groups
source_group("External Header Files" FILES ${${PROJECT_NAME}_h})
source_group("Project Related Files" FILES ${${PROJECT_NAME}_rel})

# Define output directories
set_target_properties(${PROJECT_NAME}
	PROPERTIES
	PROJECT_LABEL "Common Core Headers"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
