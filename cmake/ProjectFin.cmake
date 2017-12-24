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
source_group("External Header Files" FILES ${${PROJECT_NAME}_h})
source_group("Project Related Files" FILES ${${PROJECT_NAME}_rel})

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
