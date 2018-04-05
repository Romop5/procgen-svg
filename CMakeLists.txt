CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

project(ProcGen-SVG)

set(SVG_SOURCES
	./src/svg.cpp)
add_executable(svgcreator ${SVG_SOURCES})
target_link_libraries(svgcreator procgen)
