#ifndef SCENEMAIN_H
#define SCENEMAIN_H

#include "Scene.h"
#include "Object.h"
#include <list>

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
    void shootPlayer();
    void updatePlayerProjectile(float deltaTime);
    void randerProjectilePlayer();

private:
    Game& game;
    Player player;
    std::list<ProjectilePlayer*> projectilePlayer;// 使用指针避免栈上内存过多
    ProjectilePlayer projectilePlayerTemplate;
};

#endif