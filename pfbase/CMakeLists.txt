project(PfBase CXX)

message(STATUS "Configure ${PROJECT_NAME}")

# Set VERSION
set(${PROJECT_NAME}_VERSION_MINOR 3)
set(${PROJECT_NAME}_VERSION ${Cryptox_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR})

# Project directories
set(${PROJECT_NAME}_SRC src)
set(${PROJECT_NAME}_INCLUDE include)

# Options
# None at the moment

# Fetch sources
file(GLOB ${PROJECT_NAME}_src ${${PROJECT_NAME}_SRC}/*.cpp ${${PROJECT_NAME}_SRC}/*.h)
file(GLOB ${PROJECT_NAME}_h ${${PROJECT_NAME}_INCLUDE}/*.h)

include_directories(${${PROJECT_NAME}_INCLUDE})

add_library(${PROJECT_NAME} SHARED
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
)

# Public header
source_group("External Header Files" FILES ${${PROJECT_NAME}_h})

# Define output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
	OUTPUT_NAME "pfbase"
	PROJECT_LABEL "Project File Base"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Link static libraries
target_link_libraries(${PROJECT_NAME}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_LIBRARIES}
)

# Expose public includes to other
# subprojects through cache variable
set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${${PROJECT_NAME}_INCLUDE}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
