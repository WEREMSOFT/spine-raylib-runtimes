//
// Created by Pablo Weremczuk on 12/22/19.
//

#ifndef RAYLIBTEST_GAME_H
#define RAYLIBTEST_GAME_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

typedef struct GameContext {
    Vector2 screen_size;
    Camera3D camera;
    ecs_world_t *world;
} GameContext;

GameContext game_init_context() {
    printf("Initializando\n");

    GameContext return_value = {0};

    return_value.screen_size.x = SCREEN_WIDTH;
    return_value.screen_size.y = SCREEN_HEIGHT;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - 3d camera free");

    // Define the camera to look into our 3d world
    return_value.camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
    return_value.camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    return_value.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    return_value.camera.fovy = 45.0f;                                // Camera field-of-view Y
    return_value.camera.type = CAMERA_PERSPECTIVE;                   // Camera mode type

    SetCameraMode(return_value.camera, CAMERA_FREE); // Set a free camera mode

//    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second

    return return_value;
}

void game_fini(GameContext *game_context) {
    CloseWindow();
}

#endif //RAYLIBTEST_GAME_H
