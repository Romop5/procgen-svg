CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

project(ProcGen-SVG)

add_subdirectory(procgen)

set(SVG_SOURCES
	./src/svg.cpp)

# Creat final .exe with linked procgen library
add_executable(svgcreator ${SVG_SOURCES})
target_link_libraries(svgcreator procgen)

set(TURTLE_SOURCES
	./src/turtle.cpp)

# Creat final .exe with linked procgen library
add_executable(turtle ${TURTLE_SOURCES})
target_link_libraries(turtle procgen)
