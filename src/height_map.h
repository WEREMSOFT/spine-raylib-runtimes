//
// Created by Pablo Weremczuk on 12/22/19.
//
#pragma once
#ifndef RAYLIBTEST_HEIGHT_MAP_H
#define RAYLIBTEST_HEIGHT_MAP_H

void height_map_render(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Model, models,1);
    ECS_COLUMN(rows, Vector3, positions,2);
    GameContext* context = (GameContext*)ecs_get_system_context(rows->world, rows->system);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode3D(context->camera);

    for (int i = 0; i < rows->count; i ++) {
        DrawModel(models[i], positions[i], 1.0f, RED);
    }

    DrawGrid(20, 1.0f);
    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
}

void init_model(ecs_rows_t *rows){
    ECS_COLUMN(rows, Model, models,1);
    ECS_COLUMN(rows, Vector3, positions,2);


    Image perlin_noise_image = GenImagePerlinNoise(128, 128, 50, 50, 4.0f);
    Texture2D texture = LoadTextureFromImage(perlin_noise_image);

    for(int i = 0; i < rows->count; i++){
        Mesh mesh = GenMeshHeightmap(perlin_noise_image, (Vector3){ 16, 8, 16 });    // Generate heightmap mesh (RAM and VRAM)
        models[i] = LoadModelFromMesh(mesh);                          // Load model from generated mesh

        models[i].materials[0].maps[MAP_DIFFUSE].texture = texture;         // Set map diffuse texture
        positions[i] = (Vector3){ -8.0f, 0.0f, -8.0f };
    }
    UnloadImage(perlin_noise_image);
}

void fini_model(ecs_rows_t *rows){
    ECS_COLUMN(rows, Model, models,1);

    for (int i = 0; i < rows->count; i++) {
        UnloadTexture(models[i].materials[0].maps[MAP_DIFFUSE].texture);
        UnloadModel(models[i]);
    }
}

void height_map_update_camera(ecs_rows_t *rows) {
    GameContext* game_context = (GameContext *)ecs_get_system_context(rows->world, rows->system);
    static float phase = 0;
    phase += 1.0 * rows->delta_time;
    ECS_COLUMN(rows, Vector3, position, 1);
    for(int i = 0; i < rows->count; i++) {
        game_context->camera.position.x = sin(phase) * 20;
        game_context->camera.position.z = cos(phase) * 20;
        game_context->camera.position.y = 20;
    }
}

void height_map_init_world(ecs_world_t** world, GameContext *game_context, int argc, char** argv){
    *world = ecs_init_w_args(argc, argv);
    ECS_COMPONENT(*world, Model);
    ECS_COMPONENT(*world, Vector3);

    ECS_SYSTEM(*world, init_model, EcsOnAdd, Model, Vector3);
    ECS_ENTITY(*world, terrain, Model, Vector3);

    ECS_SYSTEM(*world, height_map_update_camera, EcsOnUpdate, Vector3);
    ecs_set_system_context(*world, height_map_update_camera, game_context);

    ECS_SYSTEM(*world, height_map_render, EcsOnUpdate, Model, Vector3);
    ECS_SYSTEM(*world, fini_model, EcsOnRemove, Model);
    ecs_set_system_context(*world, height_map_render, game_context);

}

#endif //RAYLIBTEST_HEIGHT_MAP_H
