cmake_minimum_required(VERSION 3.15)
project(raylibtest C)

file(COPY assets DESTINATION ./)
set(CMAKE_C_STANDARD 99)

# -lflecs_static
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
FILE(GLOB sources_spine src/spine/*)
add_executable(raylibtest ${sources} ${sources_spine})
target_include_directories(raylibtest PRIVATE lib/include)