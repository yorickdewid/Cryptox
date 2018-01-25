project(Crylib C)

message(STATUS "Configure ${PROJECT_NAME}")

file(GLOB ${PROJECT_NAME}_src *.c)
file(GLOB ${PROJECT_NAME}_h *.h)

add_library(${PROJECT_NAME} SHARED
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
)

# Define output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
	PROJECT_LABEL "CIL Standard Library"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
