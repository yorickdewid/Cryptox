project(CommonCore CXX)

message(STATUS "Configure ${PROJECT_NAME}")

# Set VERSION
set(${PROJECT_NAME}_VERSION_MINOR 12)
set(${PROJECT_NAME}_VERSION ${Cryptox_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR})

# Project directories
set(${PROJECT_NAME}_SRC src)
set(${PROJECT_NAME}_INCLUDE include)

# Fetch sources
file(GLOB ${PROJECT_NAME}_src ${${PROJECT_NAME}_SRC}/*.cpp ${${PROJECT_NAME}_SRC}/*.h)
file(GLOB ${PROJECT_NAME}_h ${${PROJECT_NAME}_INCLUDE}/*.h)

include_directories(${${PROJECT_NAME}_INCLUDE})

add_library(${PROJECT_NAME} SHARED
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
)

