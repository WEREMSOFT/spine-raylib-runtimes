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
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPTIMIZATION_FLAGS} -l:libraylib.a -lm -ldl -lpthread -lX11 -lxcb -lGL -lGLX -lXext -lGLdispatch -lXau -lXdmcp -v")
endif()

FILE(GLOB sources src/*)
#You can use different spine versions, but not in the same project
FILE(GLOB sources_spine_3_7 lib/spine/spine_3_7/src/spine/*)
FILE(GLOB sources_spine_3_8 lib/spine/spine_3_8/src/spine/*)

add_executable(example2dCat src/example_2d_cat.c ${sources_spine_3_7})
target_include_directories(example2dCat PRIVATE lib/include)
target_include_directories(example2dCat PRIVATE lib/spine/spine_3_7/include)

# add_executable(example2dOwl src/example_2d_owl.c ${sources_spine_3_8})
# target_include_directories(example2dOwl PRIVATE lib/include)
# target_include_directories(example2dOwl PRIVATE lib/spine/spine_3_8/include)

# add_executable(example2d src/example_2d.c ${sources_spine_3_8})
# target_include_directories(example2d PRIVATE lib/include)
# target_include_directories(example2d PRIVATE lib/spine/spine_3_8/include)

# add_executable(example2dCatWitch src/example_2d_cat_witch.c ${sources_spine_3_7})
# target_include_directories(example2dCatWitch PRIVATE lib/include)
# target_include_directories(example2dCatWitch PRIVATE lib/spine/spine_3_7/include)

# add_executable(example3d src/example_3d.c ${sources_spine_3_8})
# target_include_directories(example3d PRIVATE lib/include)
# target_include_directories(example3d PRIVATE lib/spine/spine_3_8/include)