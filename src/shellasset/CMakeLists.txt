project(ShellAsset CXX)

message(STATUS "Configure ${PROJECT_NAME}")

file(GLOB ${PROJECT_NAME}_src *.cpp)
file(GLOB ${PROJECT_NAME}_h *.h)

add_library(${PROJECT_NAME} SHARED
	${${PROJECT_NAME}_src}
	${${PROJECT_NAME}_h}
)
