//
// Created by Pablo Weremczuk on 1/6/20.
//
#include <raylib.h>
#define MAX_TEXTURES 10
#ifndef RAYLIBTEST_TEXTUREMANAGER_H
#define RAYLIBTEST_TEXTUREMANAGER_H

Texture2D* texture_2d_create(char *path);

void texture_2d_destroy();

#endif //RAYLIBTEST_TEXTUREMANAGER_H
