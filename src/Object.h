#ifndef OBJECT_H
#define OGJECT_H

#include <SDL.h>

struct Player
{
    // 玩家材质 -x -
    SDL_Texture* texture;
    // 玩家的坐标 -？类型是两个浮点数构成的结构体-
    SDL_FPoint position = {0, 0};
    // 玩家的长宽
    int width;
    int hight;
};

#endif