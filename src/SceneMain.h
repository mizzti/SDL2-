#ifndef SCENEMAIN_H
#define SCENEMAIN_H

#include "Scene.h"
#include "Object.h"
#include "Random.h"
#include <list>
#include <random>
#include <SDL_Mixer.h>
#include <map>
#include <SDL_Ttf.h>

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

    
private:
    Random& random;
    Mix_Music* bgm;// 使用的地方创建而非Game类
    Player player;
    bool isAlive = true;
    SDL_Texture* uiHealth;// 创建ui图标
    TTF_Font* scoreFont;
    
    ProjectilePlayer projectilePlayerTemplate;
    Enemy enemyTemplate;
    ProjectileEnemy projectileEnemyTemp;
    Explosion explosionTemp;
    Item itemLifeTemp;
    
    // std::mt19937 gen;// 随机数引擎
    // std::uniform_real_distribution<float> dis;
    
    std::list<ProjectilePlayer*> projectilePlayer;// 使用指针避免栈上内存过多
    std::list<Enemy*> enemies;
    std::list<ProjectileEnemy*> projectileEnemy;
    std::list<Explosion*> explosions;
    std::list<Item*> items;
    std::map<std::string, Mix_Chunk*> sounds;

    // 更新相关
    void updatePlayer(float deltaTime);
    void updatePlayerProjectile(float deltaTime);
    void updateEnemies(float deltaTime);
    void updateEnemyProjectiles(float deltaTime);
    void updateExplosions(float deltaTime);
    void updateItem(float deltaTime);
    
    // 渲染相关
    void renderUi();
    void renderPlayer();
    void renderEnemies();
    void renderProjectilePlayer();
    void renderProjectilesEnemy();
    void renderExplosions();
    void renderItem();
    
    // 其他
    void keyboardControl(float deltaTime);
    void shootPlayer();
    void shootProjectilesEnemy(Enemy* enemy);
    void spawnEnemy(float deltaTime);
    SDL_FPoint getDirection(Enemy* enemy);
    void enemyExplode(Enemy* enemy);
    void playerExplode();
    void dropItem(Enemy* enemy);
    void playerGetItem(Item* item);
};

#endif