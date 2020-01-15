/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   This example has been created using raylib 2.5 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2019 Ramon Santamaria (@raysan5)
*
********************************************************************************************/
// Configure spine to draw double faced and to minimize zfigting artifacts
#define SP_DRAW_DOUBLE_FACED

#define SP_LAYER_SPACING 0.5
#define SP_LAYER_SPACING_BASE -1.0

#include <spine/spine.h>
#include "spine-raylib.h"
#include <stdio.h>
#include <rlgl.h>


#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

Camera camera = { 0 };

spAtlas* atlas = NULL;
spSkeletonJson* json;
spSkeletonData* skeletonData;
spSkeleton* skeleton;
spAnimationStateData* animationStateData;
spAnimationState* animationState;
Vector3 skeletonPosition = { 0, -100, 0};

void UpdateDrawFrame(void) {
    // Draw
    //----------------------------------------------------------------------------------
    UpdateCamera(&camera);
    BeginDrawing();

    ClearBackground(RAYWHITE);
    BeginMode3D(camera);
    DrawGrid(1000, 100.0f);        // Draw a grid

    spAnimationState_update(animationState, GetFrameTime());
    spAnimationState_apply(animationState, skeleton);
    spSkeleton_updateWorldTransform(skeleton);
    drawSkeleton(skeleton, skeletonPosition);
    EndMode3D();
    DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
}



int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib spine example - basic window");

    camera.position = (Vector3){ 300.0f, -300.0f, 300.0f };
    camera.target = (Vector3){ 0.0f, -100.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, -1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.type = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_ORBITAL);

    // Init spine
    atlas = spAtlas_createFromFile("assets/dragon/NewDragon.atlas", 0);
    json = spSkeletonJson_create(atlas);

    skeletonData = spSkeletonJson_readSkeletonDataFile(json, "assets/dragon/NewDragon.json");

    if (!skeletonData) {
        printf("%s\n", json->error);
        spSkeletonJson_dispose(json);
        printf("ERROR!\n");
    }

    spBone_setYDown(true);
    skeleton = spSkeleton_create(skeletonData);
    skeleton->scaleX = 0.5;
    skeleton->scaleY = 0.5;

    // Create the spAnimationStateData
    animationStateData = spAnimationStateData_create(skeletonData);
    animationState = spAnimationState_create(animationStateData);
    // Add the animation "walk" to track 0, without delay, and let it loop indefinitely
    int track = 0;
    int loop = 1;
    float delay = 0;
    spAnimationState_addAnimationByName(animationState, track, "flying", loop, delay);
    spAnimationState_addAnimationByName(animationState, 0, "flying", 1, 0);
    spAnimationState_update(animationState,.0f);
    spAnimationState_apply(animationState, skeleton);
    spSkeleton_updateWorldTransform(skeleton);

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------

    spAtlas_dispose(atlas);
    spSkeleton_dispose(skeleton);
    texture_2d_destroy(); // Destroy textures loaded by spine
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}