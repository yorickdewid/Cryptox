project(PfUpgr CXX)

message(STATUS "Configure ${PROJECT_NAME}")

file(GLOB ${PROJECT_NAME}_src *.cpp)
file(GLOB ${PROJECT_NAME}_h *.h)

include_directories(${PfBase_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
)

# Define output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
	OUTPUT_NAME "pfupgr"
	PROJECT_LABEL "Project Upgrader"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(${PROJECT_NAME}
	PfBase
)