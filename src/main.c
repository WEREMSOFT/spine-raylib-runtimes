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

GameContext game_context;
uint current_level;
ecs_world_t** levels[LEVEL_COUNT];

void handleControllers(void){
    if (IsKeyPressed(KEY_SPACE) || IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
    {
        current_level = (current_level + 1) % LEVEL_COUNT;
        game_context.world = levels[current_level];
    }
}

void game_update(void) {
    ecs_progress(game_context.world, 0);
    handleControllers();
}

int main(int argc, char* argv[]) {
    game_context = game_init_context();
    cube_init_world(&levels[LEVEL_CUBE], &game_context, argc, argv);
    height_map_init_world(&levels[LEVEL_HEIGHT_MAP], &game_context, argc, argv);

    game_context.world = levels[LEVEL_CUBE];

    current_level = LEVEL_CUBE;


#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(game_update, 0, 1);
#else

    while(ecs_progress(game_context.world, 0) && !WindowShouldClose()){
        handleControllers();
    }
    game_fini(&game_context);
    for(int i = 0; i < LEVEL_COUNT; i++){
        ecs_fini(levels[i]);
    }
#endif
}