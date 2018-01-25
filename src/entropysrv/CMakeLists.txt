project(EntropySrv CXX)

message(STATUS "Configure ${PROJECT_NAME}")

file(GLOB ${PROJECT_NAME}_src *.cpp)
file(GLOB ${PROJECT_NAME}_h *.h)

add_library(${PROJECT_NAME} SHARED
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
)

# Define output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
	OUTPUT_NAME "coilcl"
	PROJECT_LABEL "Entropy Server"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
