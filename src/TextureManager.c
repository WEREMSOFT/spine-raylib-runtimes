//
// Created by Pablo Weremczuk on 1/6/20.
//
#include "TextureManager.h"
// When a global variable is declared static, it's scope is the current file.
static Texture2D tm_textures[MAX_TEXTURES] = {0};
static int texture_index = 0;

Texture2D* texture_2d_create(char *path) {
    tm_textures[texture_index] = LoadTexture(path);
    Texture2D *t = &tm_textures[texture_index];
    texture_index++;
    return t;
}

void texture_2d_destroy() {
    while(texture_index--) UnloadTexture(tm_textures[texture_index]);
}
