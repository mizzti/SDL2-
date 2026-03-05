#ifndef SCENEMAIN_H
#define SCENEMAIN_H

#include "Scene.h"
#include "Object.h"

class Game; // 不直接inclue是为防止循环引入头文件

class SceneMain : public Scene
{
public:
    SceneMain();
    // 析构函数，使用默认实现
    ~SceneMain();

    void init() override;
    void handleEvent(SDL_Event* event) override;
    void update(float deltaTime) override;
    void render() override;
    void clean() override;

    void keyboardControl(float deltaTime);

private:
    Game& game;
    Player player;
};

#endif