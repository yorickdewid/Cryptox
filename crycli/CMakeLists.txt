project(Crycli CXX)

message(STATUS "Configure ${PROJECT_NAME}")

file(GLOB ${PROJECT_NAME}_src *.cpp *.rc)
file(GLOB ${PROJECT_NAME}_h *.h)

include_directories(${CoilCl_INCLUDE_DIRS})

# Ignore security checks
if(WIN32)
	add_definitions(-D_SCL_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_executable(${PROJECT_NAME}
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
)

# Define output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
	OUTPUT_NAME "crycli"
	PROJECT_LABEL "${Cryptox_ID} Compiler CLI Interface"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(${PROJECT_NAME}
	CoilCl
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_PROGRAM_OPTIONS_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_LIBRARIES}
)

#install(TARGETS ${PROJECT_NAME})