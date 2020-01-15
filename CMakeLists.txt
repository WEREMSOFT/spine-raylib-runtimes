cmake_minimum_required(VERSION 3.15)
project(example2d C)

file(COPY assets DESTINATION ./)
set(CMAKE_C_STANDARD 99)

if(WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(__STDC__ 1) # there's a bool collision between raylib and flecs
    link_directories ("${PROJECT_SOURCE_DIR}/lib/static/win32")
    link_libraries(winmm.lib flecs_static.lib raylib_static.lib)
else()
    set(OPTIMIZATION_FLAGS -O0)
    link_directories ("${PROJECT_SOURCE_DIR}/lib/static")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPTIMIZATION_FLAGS} -lraylib -framework OpenGL -framework GLUT -framework Cocoa -framework CoreVideo -framework IOKit -v")
endif()

FILE(GLOB sources src/*)
FILE(GLOB sources_spine lib/src/spine/*)
add_executable(example2d src/example_2d.c ${sources_spine})
target_include_directories(example2d PRIVATE lib/include)

add_executable(example3d src/example_3d.c ${sources_spine})
target_include_directories(example3d PRIVATE lib/include)