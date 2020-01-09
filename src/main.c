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
#include <spine/spine.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#include <spine/extension.h>

#include "TextureManager.h"


spAtlas* atlas = NULL;
Texture2D scarfy;
Rectangle sourceRec;
Rectangle destRec;
Vector2 origin;

spSkeletonJson* json;
spSkeletonData* skeletonData;
spSkeleton* skeleton;
spAnimationStateData* animationStateData;
spAnimationState* animationState;

void cube_init_position(ecs_rows_t * rows) {
    ECS_COLUMN(rows, Vector2, position, 1);

    for(int i = 0; i < rows->count; i++) {
        position[i].x = 0;
        position[i].y = 0;
    }
}



void cube_render(ecs_rows_t *rows) {

    ECS_COLUMN(rows, Vector2, position, 1);

    BeginDrawing();
    ClearBackground(GRAY);

//    DrawTexturePro(scarfy, sourceRec, destRec, origin, 0, WHITE);

    DrawLine((int)destRec.x, 0, (int)destRec.x, WINDOW_HEIGHT, GRAY);
    DrawLine(0, (int)destRec.y, WINDOW_WIDTH, (int)destRec.y, GRAY);

    spAnimationState_update(animationState,rows->delta_time);
    spAnimationState_apply(animationState, skeleton);
    spSkeleton_updateWorldTransform(skeleton);

    drawSkeleton(skeleton);

    DrawFPS(10, 10);
    EndDrawing();
}

void cube_init_world(ecs_world_t** world, int argc, char** argv){
    *world = ecs_init_w_args(argc, argv);
    ECS_TAG(*world, Rendereable);
    ECS_COMPONENT(*world, Vector2);

    ECS_SYSTEM(*world, cube_render, EcsOnUpdate, Vector2, Rendereable);

    ECS_SYSTEM(*world, cube_init_position, EcsOnAdd, Vector2);

    ECS_ENTITY(*world, cube, Vector2, Rendereable);
    ecs_set_target_fps(*world, 60);
}

void init_window() {
    printf("Initializando\n");

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raylib [core] example - 3d camera free");

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
}

void init_spine() {
    atlas = spAtlas_createFromFile("assets/dragon/NewDragon.atlas", 0);
    json = spSkeletonJson_create(atlas);

    skeletonData = spSkeletonJson_readSkeletonDataFile(json, "assets/dragon/NewDragon.json");
    if (!skeletonData) {
        printf("%s\n", json->error);
        spSkeletonJson_dispose(json);
        printf("ERROR!\n");
    }

    skeleton = spSkeleton_create(skeletonData);

    // TODO Pablo: Create and set animation.
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
}

void destroy_spine() {
    spAtlas_dispose(atlas);
    spSkeleton_dispose(skeleton);
}

int main(int argc, char* argv[]) {
    ecs_world_t *world;

    init_window();
    init_spine();

    world = ecs_init_w_args(argc, argv);

    scarfy = *texture_2d_create("assets/scarfy.png");        // Texture loading

    int frameWidth = scarfy.width/6;
    int frameHeight = scarfy.height;

    // Source rectangle (part of the texture to use for drawing)
    sourceRec.x = sourceRec.y = 0.0f;
    sourceRec.width = frameWidth;
    sourceRec.height = frameHeight;

    // Destination rectangle (screen rectangle where drawing part of texture)
    destRec.x = WINDOW_WIDTH/2;
    destRec.y = WINDOW_HEIGHT/2;

    destRec.width = frameWidth * 2;
    destRec.height = frameHeight * 2;

    // Origin of the texture (rotation/scale point), it's relative to destination rectangle size
    origin.x = frameWidth;
    origin.y = frameHeight;


    ECS_TAG(world, Rendereable);
    ECS_COMPONENT(world, Vector2);

    ECS_SYSTEM(world, cube_render, EcsOnUpdate, Vector2, Rendereable);
    ECS_SYSTEM(world, cube_init_position, EcsOnAdd, Vector2);

    ECS_ENTITY(world, cube, Vector2, Rendereable);
    ecs_set_target_fps(world, 60);

    while(ecs_progress(world, 0) && !WindowShouldClose());

    ecs_fini(world);
    destroy_spine();
    UnloadTexture(scarfy);        // Texture unloading
    texture_2d_destroy();
    CloseWindow();

    return 0;
}