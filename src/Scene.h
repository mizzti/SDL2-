#ifndef SCENE_H
#define SCENE_H

#include <SDL.h>

class Game;

class Scene
{
public:
    Scene();
    virtual ~Scene() = default; // 必须是虚函数，否则派生类无法释放内存

    virtual void init() = 0;
    virtual void handleEvent(SDL_Event* event) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void clean() = 0;

protected:
    Game& game;
};

#endif