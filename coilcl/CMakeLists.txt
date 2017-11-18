project(CoilCl CXX)

message(STATUS "Configure ${PROJECT_NAME}")

# Set VERSION
set(${PROJECT_NAME}_VERSION_MINOR 12)
set(${PROJECT_NAME}_VERSION ${Cryptox_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR})

# Project directories
set(${PROJECT_NAME}_SRC src)
set(${PROJECT_NAME}_INCLUDE include)
set(${PROJECT_NAME}_RESOURCE res)

# Options
option(${Cryptox_ID}_CIL_BUILD_INTERPRETER "Build with interpreter support" ON)
option(${Cryptox_ID}_CIL_BUILD_VM "Build with VM support" ON)

# Fetch sources
file(GLOB ${PROJECT_NAME}_src ${${PROJECT_NAME}_SRC}/*.cpp ${${PROJECT_NAME}_SRC}/*.h)
file(GLOB ${PROJECT_NAME}_h ${${PROJECT_NAME}_INCLUDE}/*.h)
file(GLOB ${PROJECT_NAME}_rel ${${PROJECT_NAME}_RESOURCE}/*.rc)

include_directories(${${PROJECT_NAME}_INCLUDE})

add_library(${PROJECT_NAME} SHARED
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
    ${${PROJECT_NAME}_rel}
)

# File groups
source_group("External Header Files" FILES ${${PROJECT_NAME}_h})
source_group("Project Related Files" FILES ${${PROJECT_NAME}_rel})

# Define output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
	OUTPUT_NAME "coilcl"
	PROJECT_LABEL "${Cryptox_ID} Compiler"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Expose public includes to other
# subprojects through cache variable
set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${${PROJECT_NAME}_INCLUDE}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
