#ifndef SCENEMAIN_H
#define SCENEMAIN_H

#include "Scene.h"
#include "Object.h"
#include <list>
#include <random>

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
    
    void updatePlayer(float deltaTime);
    void updatePlayerProjectile(float deltaTime);
    void updateEnemies(float deltaTime);
    void updateEnemyProjectiles(float deltaTime);
    
    void renderPlayer();
    void renderEnemies();
    void renderProjectilePlayer();
    void renderProjectilesEnemy();
    
    void spawnEnemy(float deltaTime);
    void shootProjectilesEnemy(Enemy* enemy);
    SDL_FPoint getDirection(Enemy* enemy);
    void enemyExplode(Enemy* enemy);

private:
    Game& game;
    Player player;
    bool isAlive = true;

    ProjectilePlayer projectilePlayerTemplate;
    Enemy enemyTemplate;
    ProjectileEnemy projectileEnemyTemp;
    
    std::mt19937 gen;// 随机数引擎
    std::uniform_real_distribution<float> dis;
    
    std::list<ProjectilePlayer*> projectilePlayer;// 使用指针避免栈上内存过多
    std::list<Enemy*> enemies;
    std::list<ProjectileEnemy*> projectileEnemy;

};

#endif