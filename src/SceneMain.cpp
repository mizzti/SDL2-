#include "SceneMain.h"
#include "Game.h"
// 导入IMG库，-x 库名-
#include <SDL_image.h>
#include <SDL.h>
#include <random>

SceneMain::SceneMain() : game(Game::getInstance())
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::init()
{
    // 生成随机数种子
    std::random_device rd;
    // 生成随机数引擎
    gen = std::mt19937(rd());// -x rd()：重载了(）,可以直接进行调用-
    // 创建指定分布
    dis = std::uniform_real_distribution<float>(0, 1);

    // 初始化player -x -
    player.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/SpaceShip.png");
    // 获取材质的长宽作为player的长宽，注意float到int* -x 设置的是玩家的宽高，不是位置-
    SDL_QueryTexture(player.texture, NULL, NULL, &player.width, &player.hight);
    // -x 玩家位置显示错误的原因在于缩小的对象错了，应该缩小玩家对象的长宽，而不是下方渲染时放置玩家的长方形框-
    player.hight /= 4;
    player.width /= 4;
    // 设置玩家初始坐标 -x 忘了- -注：这里的发生了低精度到高精度的隐式转换，所以无需显示转换-
    player.position.x = game.getWindowWidth() / 2 - player.width / 2;
    player.position.y = game.getWindowHight() - player.hight;

    // 初始化玩家子弹
    projectilePlayerTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/laser-3.png");
    SDL_QueryTexture(projectilePlayerTemplate.texture, NULL, NULL, &projectilePlayerTemplate.width, &projectilePlayerTemplate.hight);
    projectilePlayerTemplate.width /= 4;
    projectilePlayerTemplate.hight /= 4;

    // 初始化敌机
    enemyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/insect-2.png");
    SDL_QueryTexture(enemyTemplate.texture, NULL, NULL, &enemyTemplate.wight, &enemyTemplate.hight);
    enemyTemplate.wight /= 4;
    enemyTemplate.hight /= 4;

    // 初始化敌机子弹
    projectileEnemyTemp.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bullet-2.png");
    SDL_QueryTexture(projectileEnemyTemp.texture, NULL, NULL, &projectileEnemyTemp.wight, &projectileEnemyTemp.hight);
    projectileEnemyTemp.wight /= 4;
    projectileEnemyTemp.hight /= 4;

}

void SceneMain::handleEvent(SDL_Event* event)
{
}

void SceneMain::update(float deltaTime)
{
    keyboardControl(deltaTime);
    // 更新玩家子弹
    updatePlayerProjectile(deltaTime);
    // 产生敌机
    spawnEnemy(deltaTime);
    // 更新敌机位置
    updateEnemies(deltaTime);
    // 更新敌机子弹
    updateEnemyProjectiles(deltaTime);
}

void SceneMain::render()
{
    // 先渲染子弹，渲染会按照顺序叠加渲染（类似图层）
    randerProjectilePlayer();

    // -x 高精度float转换到低精度的int，会发生数据丢失，C++禁止这样的隐式转换，所以要显示转换-
    SDL_Rect rect = {
        static_cast<int>(player.position.x),
        static_cast<int>(player.position.y),
        player.width,
        player.hight
    };
    // 渲染图片 -x 最后一个参数是放置的位置-
    SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &rect);
    // 渲染敌机子弹
    randerProjectilesEnemy();
    // 渲染敌机
    randerEnemies();
}

void SceneMain::clean()
{
    // 清理容器
    for (auto& projectile : projectilePlayer)
    {
        if (projectile != nullptr)
        {
            delete projectile;
        }
    }
    projectilePlayer.clear();

    for (auto& enemy: enemies)
    {
        if (enemy != nullptr)
        {
            delete enemy;
        }
    }
    enemies.clear();

    for (auto& projsEnemy : projectileEnemy)
    {
        if (projsEnemy != nullptr)
        {
            delete projsEnemy;
        }
    }
    projectileEnemy.clear();

    // 释放材质 
    if (player.texture != nullptr)
    {
        // 释放图片材质
        SDL_DestroyTexture(player.texture);
        player.texture = nullptr;// 防止悬空指针
    }
    if (projectilePlayerTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(projectilePlayerTemplate.texture);
        projectilePlayerTemplate.texture = nullptr;
    }
    if (enemyTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(enemyTemplate.texture);
        enemyTemplate.texture = nullptr;
    }
    if (projectileEnemyTemp.texture != nullptr)
    {
        SDL_DestroyTexture(projectileEnemyTemp.texture);
        projectileEnemyTemp.texture = nullptr;
    }
}

void SceneMain::keyboardControl(float deltaTime)
{
    auto keyboardState = SDL_GetKeyboardState(NULL);
    // 读取键盘状态，设置速度
    if (keyboardState[SDL_SCANCODE_W])
    {
        // 设置速度
        player.position.y -= deltaTime * player.speed;
    }
    if (keyboardState[SDL_SCANCODE_S])
    {
        player.position.y += deltaTime * player.speed;
    }
    if (keyboardState[SDL_SCANCODE_A])
    {
        player.position.x -= deltaTime * player.speed;
    }
    if (keyboardState[SDL_SCANCODE_D])
    {
        player.position.x += deltaTime * player.speed;
    }

    // 设置飞机的移动范围
    if (player.position.x < 0)
    {
        player.position.x = 0;
    }
    if (player.position.x > game.getWindowWidth() - player.width)
    {
        player.position.x = game.getWindowWidth() - player.width;
    }
    if (player.position.y < 0)
    {
        player.position.y = 0;
    }
    if (player.position.y > game.getWindowHight() - player.hight)
    {
        player.position.y = game.getWindowHight() - player.hight;
    }

    // 发射子弹
    if (keyboardState[SDL_SCANCODE_SPACE])
    {
        // 控制子弹发射间隔
        auto currentTime = SDL_GetTicks();
        if ((currentTime - player.lastShootTime) > player.coolDown)
        {
            shootPlayer();
            player.lastShootTime = currentTime;
        }
    }
}

void SceneMain::shootPlayer()
{
    // 设置发射子弹 -x 发射子弹时需要加载子弹材质，频繁访问硬盘会减慢游戏速度- 
    // 使用指针是因为栈上变量的生命周期不匹配，栈上变量离开函数作用域就会被立刻销毁
    ProjectilePlayer* projectile = new ProjectilePlayer(projectilePlayerTemplate);// 拷贝构造
    // 设置子弹位置 -x 子弹位置要在发射子弹时再设置-
    projectile->position.x = player.position.x + player.width / 2 - projectile->width / 2;
    projectile->position.y = player.position.y;
    projectilePlayer.push_back(projectile);
}

void SceneMain::updatePlayerProjectile(float deltaTime)
{
    int margin = 32; // 边缘
    // 使用迭代器更新子弹
    for (auto it = projectilePlayer.begin(); it != projectilePlayer.end();)
    {
        auto projectileIt = *it;// *it得到 ProjectilePlayer*指针
        // 更新子弹的发射速度
        projectileIt->position.y -= projectileIt->speed * deltaTime;
        // 检查子弹是否超出屏幕
        if (projectileIt->position.y + margin < 0)
        {
            delete projectileIt;
            it = projectilePlayer.erase(it);
            // SDL_Log("子弹被删除");
        }
        else
        {
            ++it;
        }
    }
}

void SceneMain::randerProjectilePlayer()
{
    for (auto projectile : projectilePlayer)
    {
        SDL_Rect projectileRect = {
            static_cast<int>(projectile->position.x),
            static_cast<int>(projectile->position.y),
            projectile->width,
            projectile->hight
        };
        SDL_RenderCopy(game.getRenderer(), projectile->texture, NULL, &projectileRect);
    }
}

void SceneMain::spawnEnemy(float deltaTime)
{
    // 随机生成刷新敌机时间
    if (dis(gen) > 1 / 60.0f)
    {
        return;// 不刷新敌机
    }
    Enemy* enemy = new Enemy(enemyTemplate);
    // 生成敌机
    enemy->position.x = dis(gen) * (game.getWindowWidth() - enemy->wight); // -x 限制敌机生成的x坐标-
    enemy->position.y = - enemy->hight;
    // if (enemy->position.x < 0)
    // {
    //     enemy->position.x = 0;
    // }
    // else if (enemy->position.x > game.getWindowWidth() - enemy->wight)
    // {
    //     enemy->position.x = game.getWindowWidth() - enemy->wight;
    // }
    // 插入容器--list，会生成多个敌机
    enemies.push_back(enemy);
}

void SceneMain::randerEnemies()
{
    // 渲染敌机
    for (auto enemy: enemies)
    {
        SDL_Rect enemyRect = {
            static_cast<int>(enemy->position.x),
            static_cast<int>(enemy->position.y),
            enemy->wight,
            enemy->hight
        };
        SDL_RenderCopy(game.getRenderer(), enemy->texture, NULL, &enemyRect);
    }
}

void SceneMain::updateEnemies(float deltaTime)
{
    Uint32 currentTime = SDL_GetTicks();
    // -x 更新逻辑和生成逻辑要分开-
    for (auto it = enemies.begin(); it != enemies.end();)
    {
        auto enemy = *it;
        enemy->position.y += enemy->speed * deltaTime;
        // 超过屏幕外销毁敌机
        if (enemy->position.y > game.getWindowHight())
        {
            delete enemy;// -注： 删除enemy是因为enemy拷贝了*it，使用引用可以避免-
            // 删除链表中的敌机，更新it
            it = enemies.erase(it);
            // SDL_Log("敌机被删除");
        }
        else
        {
            // -x 判断敌机是否发射：冷却时间是否到达-
            if (currentTime - enemy->lastShootTime > enemy->coolDown)
            {
                // 生成敌机子弹 -x 应在更新敌机是生成子弹，而不是生成敌机时-
                shootProjectilesEnemy(enemy);
                enemy->lastShootTime = currentTime;
            }
            ++it;
        }
    }
}

void SceneMain::shootProjectilesEnemy(Enemy* enemy)
{
    ProjectileEnemy* projsEnemy = new ProjectileEnemy(projectileEnemyTemp);
    // -x 敌机的位置-
    projsEnemy->position.x = enemy->position.x + enemy->wight / 2 - projsEnemy->wight / 2;
    projsEnemy->position.y = enemy->position.y + enemy->hight / 2 - projsEnemy->hight / 2;
    // 设置子弹朝向
    projsEnemy->direction = getDirection(enemy);
    projectileEnemy.push_back(projsEnemy);
}

void SceneMain::updateEnemyProjectiles(float deltaTime)
{
    int margin = 32;
    for (auto it = projectileEnemy.begin(); it != projectileEnemy.end();)
    {
        auto projsEnemy = *it;
        projsEnemy->position.x += projsEnemy->direction.x * projsEnemy->speed * deltaTime;
        projsEnemy->position.y += projsEnemy->direction.y * projsEnemy->speed * deltaTime;
        
        if (projsEnemy->position.y > game.getWindowHight() + margin||
            projsEnemy->position.y < -margin ||
            projsEnemy->position.x > game.getWindowWidth() + margin ||
            projsEnemy->position.x < -margin)
        {
            // 删除当前子弹
            delete projsEnemy;
            // 删除队列中当前的子弹
            it = projectileEnemy.erase(it);
            // SDL_Log("敌人子弹被删除");
        }
        else
        {
            ++it;
        }
    }
}

void SceneMain::randerProjectilesEnemy()
{
    for (auto projsEnemy : projectileEnemy)
    {
        SDL_Rect projsRect = {
            static_cast<int>(projsEnemy->position.x),
            static_cast<int>(projsEnemy->position.y),
            projsEnemy->wight,
            projsEnemy->hight
        };
        // -注 Cpp允许向下转换-
        float angle = atan2(projsEnemy->direction.y, projsEnemy->direction.x) * 180 / M_PI - 90;
        // SDL_RenderCopy(game.getRenderer(), projsEnemy->texture, NULL, &projsRect);
        SDL_RenderCopyEx(game.getRenderer(), projsEnemy->texture, NULL, &projsRect, angle, NULL, SDL_FLIP_NONE);
    }
}

SDL_FPoint SceneMain::getDirection(Enemy* enemy)
{
    float x = (player.position.x + player.width / 2) - (enemy->position.x + enemy->wight / 2);
    float y = (player.position.y + player.hight / 2) - (enemy->position.y + enemy->hight / 2);
    float length = sqrt(x * x + y * y);

    // -注：新增：如果距离过小，避免除以0-
    if (length < 0.0001f)
    {
        return SDL_FPoint{0, 1};// 往下飞
    }

    x /= length;
    y /= length;
    return SDL_FPoint{x, y};
}
