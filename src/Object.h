#ifndef OBJECT_H
#define OGJECT_H

#include <SDL.h>

struct Player
{
    // 玩家材质 -x -
    SDL_Texture* texture = nullptr;
    // 玩家的坐标 -？类型是两个浮点数构成的结构体-
    SDL_FPoint position = {0, 0};
    // 玩家的长宽
    int width = 0;
    int hight = 0;
    int speed = 300;

    // -x 放在玩家的结构体中-
    Uint32 coolDown =  500;// 发射子弹的冷却时间，毫秒
    Uint32 lastShootTime = 0;// 记录上一次的子弹发射时间
};

struct ProjectilePlayer
{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int hight = 0;
    // -x 没有设定速度，所以子弹就无法被射出-
    int speed = 200;

};

#endif