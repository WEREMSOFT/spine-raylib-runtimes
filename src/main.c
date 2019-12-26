/*******************************************************************************************
*
*   raylib [core] example - Initialize 3d camera free
*
*   This example has been created using raylib 1.3 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2015 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif
#include "flecs.h"
#include "game.h"
#include "cube.h"
#include "height_map.h"

enum game_level {
    LEVEL_CUBE,
    LEVEL_HEIGHT_MAP,
    LEVEL_COUNT
};

int main(int argc, char* argv[]) {

    ecs_world_t** levels[LEVEL_COUNT];

    GameContext game_context = game_init_context();
    cube_init_world(&levels[LEVEL_CUBE], &game_context, argc, argv);
    height_map_init_world(&levels[LEVEL_HEIGHT_MAP], &game_context, argc, argv);

    game_context.world = levels[LEVEL_HEIGHT_MAP];

    uint current_level = LEVEL_HEIGHT_MAP;

    while(ecs_progress(game_context.world, 0) && !WindowShouldClose()){
        if (IsKeyPressed(KEY_SPACE))
        {
            current_level = (current_level + 1) % LEVEL_COUNT;
            game_context.world = levels[current_level];
        }
    }
    game_fini(&game_context);

    for(int i = 0; i < LEVEL_COUNT; i++){
        ecs_fini(levels[i]);
    }
}