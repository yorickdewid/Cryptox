project(CEXDump CXX)

message(STATUS "Configure ${PROJECT_NAME}")

# Set project info
set(${PROJECT_NAME}_VERSION_LOCAL 12)
set(${PROJECT_NAME}_DESCRIPTION "CEX Image Dump")

string(TOLOWER ${PROJECT_NAME} PRODUCT_ORIGINAL_NAME)
set(PRODUCT_VERSION_LOCAL ${${PROJECT_NAME}_VERSION_LOCAL})
set(PRODUCT_DESCRIPTION ${${PROJECT_NAME}_DESCRIPTION})

# Project directories
set(${PROJECT_NAME}_SRC src)
set(${PROJECT_NAME}_INCLUDE include)
set(${PROJECT_NAME}_RESOURCE res)

configure_file(${${PROJECT_NAME}_RESOURCE}/cexdump.rc.in ${CMAKE_CURRENT_BINARY_DIR}/cexdump.rc @ONLY)

# Fetch sources
file(GLOB ${PROJECT_NAME}_src ${${PROJECT_NAME}_SRC}/*.cpp ${${PROJECT_NAME}_SRC}/*.h)
file(GLOB ${PROJECT_NAME}_h ${${PROJECT_NAME}_INCLUDE}/*.h)
file(GLOB ${PROJECT_NAME}_rel ${${PROJECT_NAME}_RESOURCE}/*.in)
file(GLOB ${PROJECT_NAME}_res ${CMAKE_CURRENT_BINARY_DIR}/*.rc)

include_directories(${CoilCl_INCLUDE_DIRS})

# Ignore security checks
if(WIN32)
	add_definitions(-D_SCL_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_executable(${PROJECT_NAME}
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
	${${PROJECT_NAME}_rel}
	${${PROJECT_NAME}_res}
)

# File groups
source_group("External Header Files" FILES ${${PROJECT_NAME}_h})
source_group("Project Related Files" FILES ${${PROJECT_NAME}_rel})

# Define output directories
set_target_properties(${PROJECT_NAME}
	PROPERTIES
	OUTPUT_NAME "cexdump"
	PROJECT_LABEL "CEX Image Dump"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(${PROJECT_NAME}
	CoilCEX
	${Boost_PROGRAM_OPTIONS_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)