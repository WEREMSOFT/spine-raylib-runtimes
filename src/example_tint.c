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

#include <spine/spine.h>
#include "spine-raylib.h"
#include <stdio.h>
#include <rlgl.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 650

spAtlas* atlas = NULL;
spSkeletonJson* json;
spSkeletonData* skeletonData;
spSkeleton* skeleton;
spAnimationStateData* animationStateData;
spAnimationState* animationState;
Vector3 skeletonPosition = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0};

#define PETROL CLITERAL(Color){ 0, 122, 163, 255 }   // My own White (raylib logo)

void UpdateDrawFrame(void) {
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(BLANK);

    spAnimationState_update(animationState, GetFrameTime());
    spAnimationState_apply(animationState, skeleton);
    spSkeleton_updateWorldTransform(skeleton);
    drawSkeleton(skeleton, skeletonPosition, false);
    DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
}


EMSCRIPTEN_KEEPALIVE void myFunction() {
    printf("MyFunction Called\n");
}


int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    //SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib spine example - basic window");

    // Init spine
    atlas = spAtlas_createFromFile("assets/example_tint/cake.atlas", 0);
    json = spSkeletonJson_create(atlas);

    skeletonData = spSkeletonJson_readSkeletonDataFile(json, "assets/example_tint/cake.json");

    if (!skeletonData) {
        printf("%s\n", json->error);
        spSkeletonJson_dispose(json);
        printf("ERROR!\n");
    }

    spBone_setYDown(true);
    skeleton = spSkeleton_create(skeletonData);
    skeleton->scaleX = 1;
    skeleton->scaleY = 1;

    // Create the spAnimationStateData
    animationStateData = spAnimationStateData_create(skeletonData);
    animationState = spAnimationState_create(animationStateData);
    // Add the animation "walk" to track 0, without delay, and let it loop indefinitely
    int track = 0;
    int loop = 1;
    float delay = 0;
    spAnimationState_addAnimationByName(animationState, track, "stand", loop, delay);
    spAnimationState_addAnimationByName(animationState, 0, "stand", 1, 0);
    spAnimationState_update(animationState,.0f);
    spAnimationState_apply(animationState, skeleton);
    spSkeleton_updateWorldTransform(skeleton);

    SetTargetFPS(60);
    //printf("%s\n", "test");
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