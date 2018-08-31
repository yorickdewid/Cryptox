# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

function(enable_auto_test TEST_NAME)
	if(NOT PROJECT_NAME)
		message(FATAL_ERROR "Enable auto tests after project preperations")
	endif()

	# Enable tests if there are test sources
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${${PROJECT_NAME}_TEST})
		message(STATUS "Enable tests for ${PROJECT_NAME}")

		# Allow testing on the target
		enable_testing()

		if(UNIX)
			add_definitions(-DBOOST_TEST_DYN_LINK) 
		endif()

		file(GLOB ${PROJECT_NAME}_TEST_SRC ${${PROJECT_NAME}_TEST}/*.cpp)
		add_executable(${PROJECT_NAME}_unittest
			${${PROJECT_NAME}_TEST_SRC}
		)

		string(TOLOWER ${PROJECT_NAME}_unittest ${PROJECT_NAME}_OUT_EXE)
		set_target_properties(${PROJECT_NAME}_unittest
			PROPERTIES
			OUTPUT_NAME ${${PROJECT_NAME}_OUT_EXE}
			PROJECT_LABEL "${TEST_NAME}"
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
		)

		add_test(NAME ${PROJECT_NAME}_unittest
			COMMAND ${${PROJECT_NAME}_OUT_EXE}
		)

		target_link_libraries(${PROJECT_NAME}_unittest
			${PROJECT_NAME}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			${Boost_LIBRARIES}
		)
	endif()
endfunction()

