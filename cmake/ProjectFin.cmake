# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

# Expose public includes to other
# subprojects through cache variable
set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${${PROJECT_NAME}_INCLUDE}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

# File groups
set(EXTERNAL_HEADER_GROUP "External Header Files")
set(RELATED_FILES_GROUP "Project Related Files")

source_group(${RELATED_FILES_GROUP} FILES ${${PROJECT_NAME}_rel})
foreach(SOURCE IN ITEMS ${${PROJECT_NAME}_h})
	get_filename_component(SOURCE_PATH ${SOURCE} PATH)
	file(RELATIVE_PATH SOURCE_PATH_SINGLE ${CMAKE_CURRENT_SOURCE_DIR}/include ${SOURCE_PATH})
	if(SOURCE_PATH_SINGLE)
		string(REPLACE "/" "\\" GROUP_PATH ${SOURCE_PATH_SINGLE})
		source_group(${EXTERNAL_HEADER_GROUP}\\${GROUP_PATH} FILES ${SOURCE})
	else()
		source_group(${EXTERNAL_HEADER_GROUP} FILES ${SOURCE})
	endif()
endforeach()

# Figure out the target type
get_target_property(PROJECT_TARGET_TYPE ${PROJECT_NAME} TYPE)
if(PROJECT_TARGET_TYPE STREQUAL SHARED_LIBRARY)
	message(STATUS "Target compiling as shared library")
	target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_SHARED_LIBRARY)
elseif(PROJECT_TARGET_TYPE STREQUAL EXECUTABLE)
	message(STATUS "Target compiling as executable")
	target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_EXECUTABLE)
elseif(PROJECT_TARGET_TYPE STREQUAL STATIC_LIBRARY)
	message(STATUS "Target compiling as static library")
	target_compile_definitions(${PROJECT_NAME} PRIVATE TARGET_STATIC_LIBRARY)
else()
	message(FATAL_ERROR "Unknown target ${PROJECT_TARGET_TYPE}")
endif()

# On all projects, include the Cry Framework
if(NOT ${PROJECT_NAME} STREQUAL CryWork)
	target_link_libraries(${PROJECT_NAME}
		CryWork
	)
endif()

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
