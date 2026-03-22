#ifndef OBJECT_H
#define OBJECT_H

#include <SDL.h>

enum class ItemType
{
    Life,
    Shield,
    Time
};

struct Player
{
    // 玩家材质 [BUG] -
    SDL_Texture* texture = nullptr;
    // 玩家的坐标 [BUG] 类型是两个浮点数构成的结构体-
    SDL_FPoint position = {0, 0};
    // 玩家的长宽
    int width = 0;
    int height = 0;
    int speed = 300;

    // [BUG] 放在玩家的结构体中-
    Uint32 coolDown =  400;// 发射子弹的冷却时间，毫秒
    Uint32 lastShootTime = 0;// 记录上一次的子弹发射时间

    int maxHealth = 3;
    int curHealth = maxHealth;
};

struct ProjectilePlayer
{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    // [BUG] 没有设定速度，所以子弹就无法被射出-
    int speed = 300;
    int demage = 1;
};

struct Enemy
{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 150;

    Uint32 coolDown = 2000;
    Uint32 lastShootTime = 0;

    int health = 2;
};

struct ProjectileEnemy
{
    SDL_Texture* texture;
    SDL_FPoint position = {0, 0};
    // 子弹朝向
    SDL_FPoint direction = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 180;

    int demage = 1;
};

struct Explosion
{
    SDL_Texture* texture;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int curFrame = 0;
    int totalFrame = 0;
    Uint32 startTime = 0;
    int FPS = 10;
};

struct Item
{
    SDL_Texture* texture;
    SDL_FPoint position = {0, 0};
    SDL_FPoint direction = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 150;
    ItemType type = ItemType::Life;
    int bounceCount = 3;
};

struct Background
{
    SDL_Texture* texture;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 30;
    float yOffset = 0;// y轴的偏移量
};

#endif