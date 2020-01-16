cmake_minimum_required(VERSION 3.15)
project(spine-raylib-runtime C)

file(COPY assets DESTINATION ./)
set(CMAKE_C_STANDARD 99)

# set compiler / linker settings for the platform
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    link_directories("${PROJECT_SOURCE_DIR}/lib/static/win32")
    link_libraries(winmm.lib raylib_static.lib)
elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    set(OPTIMIZATION_FLAGS -O0)
    link_directories("${PROJECT_SOURCE_DIR}/lib/static/mac")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPTIMIZATION_FLAGS} -lraylib -framework OpenGL -framework GLUT -framework Cocoa -framework CoreVideo -framework IOKit -v")
elseif(CMAKE_SYSTEM_NAME STREQUAL Linux)
endif()

# grab all the source in both the example and the spine lib
FILE(GLOB sources src/*)
FILE(GLOB sources_spine lib/src/spine/*)

# build 2d example
add_executable(example2d src/example_2d.c ${sources_spine})
target_include_directories(example2d PRIVATE lib/include)

# build 3d example
add_executable(example3d src/example_3d.c ${sources_spine})
target_include_directories(example3d PRIVATE lib/include)