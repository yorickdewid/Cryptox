project(CommonCore CXX)

message(STATUS "Configure ${PROJECT_NAME}")

# Set VERSION
set(${PROJECT_NAME}_VERSION_MINOR 12)
set(${PROJECT_NAME}_VERSION ${Cryptox_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR})

# Fetch sources
file(GLOB ${PROJECT_NAME}_h *.h)

# Public header
source_group("External Header Files" FILES ${${PROJECT_NAME}_h})

add_custom_target(${PROJECT_NAME}
	SOURCES ${${PROJECT_NAME}_h}
)

# Expose public includes to other
# subprojects through cache variable
set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${${PROJECT_NAME}_h}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)