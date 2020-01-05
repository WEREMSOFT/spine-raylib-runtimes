//
// Created by Pablo Weremczuk on 12/22/19.
//

#include "game.h"
#include <math.h>

#ifndef RAYLIBTEST_CUBE_H
#define RAYLIBTEST_CUBE_H

void cube_init_position(ecs_rows_t * rows) {
    ECS_COLUMN(rows, Vector3, position, 1);

    for(int i = 0; i < rows->count; i++) {
        position[i].x = 0;
        position[i].y = 0;
        position[i].z = 0;
    }
}

void cube_render(ecs_rows_t *rows) {
    GameContext* context = (GameContext*)ecs_get_system_context(rows->world, rows->system);

    ECS_COLUMN(rows, Vector3, position, 1);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode3D(context->camera);

    for (int i = 0; i < rows->count; i ++) {
        DrawCube(position[i], 2.0f, 2.0f, 2.0f, CLITERAL(Color){11, 110, 176, 255});
//        DrawSphere(position[i], 1.0f, CLITERAL(Color){11, 110, 176, 255});
        DrawCubeWires(position[i], 2.0f, 2.0f, 2.0f, MAROON);
    }

    DrawGrid(10, 1.0f);
    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
}

void cube_update_position(ecs_rows_t *rows) {
    GameContext* game_context = (GameContext *)ecs_get_system_context(rows->world, rows->system);
    static float phase = 0;
    phase += 1.0 * rows->delta_time;
    ECS_COLUMN(rows, Vector3, position, 1);
    for(int i = 0; i < rows->count; i++) {
        game_context->camera.position.x = sin(phase) * 10;
        game_context->camera.position.z = cos(phase) * 10;
        game_context->camera.position.y = 10;

    }
}

void cube_init_world(ecs_world_t** world, GameContext *gameContext, int argc, char** argv){
    *world = ecs_init_w_args(argc, argv);
    ECS_TAG(*world, Rendereable);
    ECS_COMPONENT(*world, Vector3);

    ECS_SYSTEM(*world, cube_render, EcsOnUpdate, Vector3, Rendereable);
    ecs_set_system_context(*world, cube_render, gameContext);

    ECS_SYSTEM(*world, cube_init_position, EcsOnAdd, Vector3);

    ECS_SYSTEM(*world, cube_update_position, EcsOnUpdate, Vector3);
    ecs_set_system_context(*world, cube_update_position, gameContext);

    ECS_ENTITY(*world, cube, Vector3, Rendereable);
    ecs_set_target_fps(*world, 120);
}

#endif //RAYLIBTEST_CUBE_H
