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
#include "flecs.h"
#include <math.h>

Camera3D camera;

void cube_init_position(ecs_rows_t * rows) {
    ECS_COLUMN(rows, Vector3, position, 1);

    for(int i = 0; i < rows->count; i++) {
        position[i].x = 0;
        position[i].y = 0;
        position[i].z = 0;
    }
}



void cube_render(ecs_rows_t *rows) {

    ECS_COLUMN(rows, Vector3, position, 1);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode3D(camera);

    for (int i = 0; i < rows->count; i ++) {
        DrawCube(position[i], 2.0f, 2.0f, 2.0f, CLITERAL(Color){11, 110, 176, 255});
        DrawCubeWires(position[i], 2.0f, 2.0f, 2.0f, MAROON);
    }

    DrawGrid(10, 1.0f);
    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
}

void cube_update_position(ecs_rows_t *rows) {
    static float phase = 0;
    phase += 1.0 * rows->delta_time;
    ECS_COLUMN(rows, Vector3, position, 1);
    for(int i = 0; i < rows->count; i++) {
        camera.position.x = sin(phase) * 10;
        camera.position.z = cos(phase) * 10;
        camera.position.y = 10;

    }
}

void cube_init_world(ecs_world_t** world, int argc, char** argv){
    *world = ecs_init_w_args(argc, argv);
    ECS_TAG(*world, Rendereable);
    ECS_COMPONENT(*world, Vector3);

    ECS_SYSTEM(*world, cube_render, EcsOnUpdate, Vector3, Rendereable);

    ECS_SYSTEM(*world, cube_init_position, EcsOnAdd, Vector3);

    ECS_SYSTEM(*world, cube_update_position, EcsOnUpdate, Vector3);

    ECS_ENTITY(*world, cube, Vector3, Rendereable);
    ecs_set_target_fps(*world, 120);
}

void init_camera() {
    printf("Initializando\n");

    InitWindow(800, 600, "raylib [core] example - 3d camera free");

    // Define the camera to look into our 3d world
    camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.type = CAMERA_PERSPECTIVE;                   // Camera mode type

    SetCameraMode(camera, CAMERA_FREE); // Set a free camera mode

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
}



int main(int argc, char* argv[]) {
    ecs_world_t *world;

    init_camera();

    world = ecs_init_w_args(argc, argv);

    ECS_TAG(world, Rendereable);
    ECS_COMPONENT(world, Vector3);

    ECS_SYSTEM(world, cube_render, EcsOnUpdate, Vector3, Rendereable);

    ECS_SYSTEM(world, cube_init_position, EcsOnAdd, Vector3);

    ECS_SYSTEM(world, cube_update_position, EcsOnUpdate, Vector3);

    ECS_ENTITY(world, cube, Vector3, Rendereable);
    ecs_set_target_fps(world, 60);

    while(ecs_progress(world, 0) && !WindowShouldClose());

    ecs_fini(world);
    CloseWindow();
}