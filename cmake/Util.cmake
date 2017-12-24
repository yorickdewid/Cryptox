# Copyright (c) 2017 Quenza Inc. All rights reserved.
# Copyright (c) 2018 Blub Corp. All rights reserved.
#
# This file is part of the Cryptox project.
#
# Use of this source code is governed by a private license
# that can be found in the LICENSE file. Content can not be 
# copied and/or distributed without the express of the author.

function(enable_unsecure_crt)
	if(WIN32)
		add_definitions(-D_SCL_SECURE_NO_WARNINGS)
		add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	endif()
endfunction()

macro(project_version PROJECT VERSION)
	set(${PROJECT}_VERSION_LOCAL ${VERSION})
endmacro()

macro(project_description PROJECT DESC)
	set(${PROJECT}_DESCRIPTION ${DESC})
endmacro()
