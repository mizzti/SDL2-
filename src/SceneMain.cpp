#include "SceneMain.h"
#include "Game.h"
// 导入IMG库，[BUG] 库名-
#include <SDL_image.h>
#include <SDL.h>
#include <random>

SceneMain::SceneMain() : game(Game::getInstance()), random(Random::getInstance())
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::init()
{
    // 初始化player [BUG] -
    player.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/SpaceShip.png");
    // 获取材质的长宽作为player的长宽，注意float到int* [BUG] 设置的是玩家的宽高，不是位置-
    SDL_QueryTexture(player.texture, NULL, NULL, &player.width, &player.height);
    // [BUG] 玩家位置显示错误的原因在于缩小的对象错了，应该缩小玩家对象的长宽，而不是下方渲染时放置玩家的长方形框-
    player.height /= 4;
    player.width /= 4;
    // 设置玩家初始坐标 [NOTE]：这里的发生了int到float不会丢失数据，所以无需显示转换-
    player.position.x = game.getWindowWidth() / 2 - player.width / 2;
    player.position.y = game.getWindowHeight() - player.height;

    // 初始化玩家子弹
    projectilePlayerTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/laser-3.png");
    SDL_QueryTexture(projectilePlayerTemplate.texture, NULL, NULL, &projectilePlayerTemplate.width, &projectilePlayerTemplate.height);
    projectilePlayerTemplate.width /= 2;
    projectilePlayerTemplate.height /= 2;

    // 初始化敌机
    enemyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/insect-2.png");
    SDL_QueryTexture(enemyTemplate.texture, NULL, NULL, &enemyTemplate.width, &enemyTemplate.height);
    enemyTemplate.width /= 4;
    enemyTemplate.height /= 4;

    // 初始化敌机子弹
    projectileEnemyTemp.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bullet-2.png");
    SDL_QueryTexture(projectileEnemyTemp.texture, NULL, NULL, &projectileEnemyTemp.width, &projectileEnemyTemp.height);
    projectileEnemyTemp.width /= 2;
    projectileEnemyTemp.height /= 2;

    // 初始化爆炸
    explosionTemp.texture = IMG_LoadTexture(game.getRenderer(), "assets/effect/explosion.png");
    SDL_QueryTexture(explosionTemp.texture, NULL, NULL, &explosionTemp.width, &explosionTemp.height);
    explosionTemp.totalFrame = explosionTemp.width / explosionTemp.height;
    explosionTemp.width = explosionTemp.height; // 矩形

    // 初始化掉落物品--增加生命值
    itemLifeTemp.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bonus_life.png");
    SDL_QueryTexture(itemLifeTemp.texture, NULL, NULL, &itemLifeTemp.width, &itemLifeTemp.height);
    itemLifeTemp.width /= 2;
    itemLifeTemp.height /= 2;
}

void SceneMain::handleEvent(SDL_Event* event)
{
}

void SceneMain::update(float deltaTime)
{
    updatePlayer(deltaTime);
    keyboardControl(deltaTime);
    // 更新玩家子弹
    updatePlayerProjectile(deltaTime);
    // 产生敌机
    spawnEnemy(deltaTime);
    // 更新敌机位置
    updateEnemies(deltaTime);
    // 更新敌机子弹
    updateEnemyProjectiles(deltaTime);
    updateExplosions(deltaTime);
}

void SceneMain::render()
{
    // 先渲染子弹，渲染会按照顺序叠加渲染（类似图层）
    renderProjectilePlayer();

    renderPlayer();

    // 渲染敌机子弹
    renderProjectilesEnemy();
    // 渲染敌机
    renderEnemies();
    renderExplosions();
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

    for (auto& explode : explosions)
    {
        if (explode != nullptr)
        {
            delete explode;
        }
    }
    explosions.clear();

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
    if (explosionTemp.texture != nullptr)
    {
        SDL_DestroyTexture(explosionTemp.texture);
        explosionTemp.texture = nullptr;
    }
}

void SceneMain::keyboardControl(float deltaTime)
{
    if (!isAlive)
    {
        return;
    }
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
    if (player.position.y > game.getWindowHeight() - player.height)
    {
        player.position.y = game.getWindowHeight() - player.height;
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
    // 设置发射子弹 [LEARN] 发射子弹时需要加载子弹材质，频繁访问硬盘会减慢游戏速度- 
    // 使用指针是因为栈上变量的生命周期不匹配，栈上变量离开函数作用域就会被立刻销毁
    ProjectilePlayer* projectile = new ProjectilePlayer(projectilePlayerTemplate);// 拷贝构造
    // 设置子弹位置 [BUG] 子弹位置要在发射子弹时再设置-
    projectile->position.x = player.position.x + player.width / 2 - projectile->width / 2;
    projectile->position.y = player.position.y;
    projectilePlayer.push_back(projectile);
}

void SceneMain::updatePlayer(float)
{
    if (!isAlive)
    {
        return;
    }

    if (player.health <= 0)
    {
        playerExplode();
    }
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
            bool hit = false;
            // 检测子弹是否命中
            // [OPT] 碰撞检测期间子弹位置是不变的，移动到循环外减少重复计算
            SDL_Rect projRect = {
                static_cast<int>(projectileIt->position.x),
                static_cast<int>(projectileIt->position.y),
                projectileIt->width,
                projectileIt->height
            };
            for (Enemy* enemy : enemies)
            {
                SDL_Rect enemyRect = {
                    static_cast<int>(enemy->position.x),
                    static_cast<int>(enemy->position.y),
                    enemy->width,
                    enemy->height
                };
                if (isAlive && SDL_HasIntersection(&enemyRect, &projRect))
                {
                    enemy->health -= projectileIt->demage;
                    delete projectileIt;
                    it = projectilePlayer.erase(it);
                    hit = true;
                    break;
                }
            }
            if (!hit)
            {
                ++it;
            }
        }
    }
}

void SceneMain::updateEnemies(float deltaTime)
{
    Uint32 currentTime = SDL_GetTicks();
    // [BUG] 更新逻辑和生成逻辑要分开-
    for (auto it = enemies.begin(); it != enemies.end();)
    {
        auto enemy = *it;
        enemy->position.y += enemy->speed * deltaTime;
        // 超过屏幕外销毁敌机
        if (enemy->position.y > game.getWindowHeight())
        {
            delete enemy;// [NOTE]： 删除enemy是因为enemy拷贝了*it，使用引用可以避免-
            // 删除链表中的敌机，更新it
            it = enemies.erase(it);
            // SDL_Log("敌机被删除");
        }
        else
        {
            // [BUG] 判断敌机是否发射：通过冷却时间-
            if (isAlive && currentTime - enemy->lastShootTime > enemy->coolDown)
            {
                // 生成敌机子弹 [BUG] 应在更新敌机是生成子弹，而不是生成敌机时-
                shootProjectilesEnemy(enemy);
                enemy->lastShootTime = currentTime;
            }
            // [LEARN] 也可以写在updatePlayer中，碰撞检测
            SDL_Rect enemyRect = {
                static_cast<int>(enemy->position.x),
                static_cast<int>(enemy->position.y),
                enemy->width,
                enemy->height,
            };
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x),
                static_cast<int>(player.position.y),
                player.width,
                player.height,
            };
            if (isAlive && SDL_HasIntersection(&enemyRect, &playerRect))
            {
                enemy->health = 0;
                player.health -= 1;
            }

            if (enemy->health <= 0)
            {
                enemyExplode(enemy);
                spawnLifeItem(enemy);
                it = enemies.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

void SceneMain::updateEnemyProjectiles(float deltaTime)
{
    int margin = 32;
    for (auto it = projectileEnemy.begin(); it != projectileEnemy.end();)
    {
        auto projsEnemy = *it;
        projsEnemy->position.x += projsEnemy->direction.x * projsEnemy->speed * deltaTime;
        projsEnemy->position.y += projsEnemy->direction.y * projsEnemy->speed * deltaTime;
        
        if (projsEnemy->position.y > game.getWindowHeight() + margin||
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
            // 判断敌机子弹是否打中玩家
            SDL_Rect projRect = {
                static_cast<int>(projsEnemy->position.x),
                static_cast<int>(projsEnemy->position.y),
                projsEnemy->width,
                projsEnemy->height,
            };
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x),
                static_cast<int>(player.position.y),
                player.width,
                player.height,
            };
            if (isAlive && SDL_HasIntersection(&projRect, &playerRect))
            {
                player.health -= projsEnemy->demage;
                delete projsEnemy;
                it = projectileEnemy.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

void SceneMain::updateExplosions(float)
{
    Uint32 curTime = SDL_GetTicks();
    for (auto it = explosions.begin(); it != explosions.end();)
    {
        Explosion* explode = *it;
        // [LEARN] 爆炸时间差除以每帧时长等于当前帧，每帧时长等于1/帧数，即乘以帧数。毫秒转秒
        explode->curFrame = (curTime - explode->startTime) * explode->FPS / 1000;
        if (explode->curFrame >= explode->totalFrame)
        {
            delete explode;
            it = explosions.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void SceneMain::spawnEnemy(float)
{
    // 随机生成刷新敌机时间
    if (random.getFloat() > 1 / 60.0f)
    {
        return;// 不刷新敌机
    }
    Enemy* enemy = new Enemy(enemyTemplate);
    // 生成敌机
    enemy->position.x = random.getFloat() * (game.getWindowWidth() - enemy->width); // [BUG] 限制敌机生成的x坐标-
    enemy->position.y = - enemy->height;
    // 插入容器--list，会生成多个敌机
    enemies.push_back(enemy);
}


void SceneMain::renderProjectilePlayer()
{
    for (auto projectile : projectilePlayer)
    {
        SDL_Rect projectileRect = {
            static_cast<int>(projectile->position.x),
            static_cast<int>(projectile->position.y),
            projectile->width,
            projectile->height
        };
        SDL_RenderCopy(game.getRenderer(), projectile->texture, NULL, &projectileRect);
    }
}

void SceneMain::renderPlayer()
{
    if (isAlive)
    {
        // [BUG] 高精度float转换到低精度的int，会发生数据丢失，C++禁止这样的隐式转换，所以要显示转换-
        SDL_Rect rect = {
            static_cast<int>(player.position.x),
            static_cast<int>(player.position.y),
            player.width,
            player.height
        };
        // 渲染图片 [BUG] 最后一个参数是放置的位置-
        SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &rect);
    }
}

void SceneMain::renderEnemies()
{
    // 渲染敌机
    for (auto enemy: enemies)
    {
        SDL_Rect enemyRect = {
            static_cast<int>(enemy->position.x),
            static_cast<int>(enemy->position.y),
            enemy->width,
            enemy->height
        };
        SDL_RenderCopy(game.getRenderer(), enemy->texture, NULL, &enemyRect);
    }
}

void SceneMain::renderProjectilesEnemy()
{
    for (auto projsEnemy : projectileEnemy)
    {
        SDL_Rect projsRect = {
            static_cast<int>(projsEnemy->position.x),
            static_cast<int>(projsEnemy->position.y),
            projsEnemy->width,
            projsEnemy->height
        };
        // [NOTE] 都有小数部分，只是精度不同，允许隐式转换
        float angle = atan2(projsEnemy->direction.y, projsEnemy->direction.x) * 180 / M_PI - 90;
        // SDL_RenderCopy(game.getRenderer(), projsEnemy->texture, NULL, &projsRect);
        SDL_RenderCopyEx(game.getRenderer(), projsEnemy->texture, NULL, &projsRect, angle, NULL, SDL_FLIP_NONE);
    }
}

void SceneMain::renderExplosions()
{
    for (auto explode : explosions)
    {
        // 渲染图片的坐标和长宽
        SDL_Rect src = {explode->curFrame * explode->width, 0, explode->width, explode->height};
        SDL_Rect dst = {
            static_cast<int>(explode->position.x),
            static_cast<int>(explode->position.y),
            explode->width * 2,
            explode->height * 2
        };
        SDL_RenderCopy(game.getRenderer(), explode->texture, &src, &dst);
    }
}

void SceneMain::shootProjectilesEnemy(Enemy* enemy)
{
    ProjectileEnemy* projsEnemy = new ProjectileEnemy(projectileEnemyTemp);
    // [BUG] 敌机的位置-
    projsEnemy->position.x = enemy->position.x + enemy->width / 2 - projsEnemy->width / 2;
    projsEnemy->position.y = enemy->position.y + enemy->height / 2 - projsEnemy->height / 2;
    // 设置子弹朝向
    projsEnemy->direction = getDirection(enemy);
    projectileEnemy.push_back(projsEnemy);
}

SDL_FPoint SceneMain::getDirection(Enemy* enemy)
{
    float x = (player.position.x + player.width / 2) - (enemy->position.x + enemy->width / 2);
    float y = (player.position.y + player.height / 2) - (enemy->position.y + enemy->height / 2);
    float length = sqrt(x * x + y * y);

    // [NOTE]：新增：如果距离过小，避免除以0-
    if (length < 0.0001f)
    {
        return SDL_FPoint{0, 1};// 往下飞
    }

    x /= length;
    y /= length;
    return SDL_FPoint{x, y};
}

void SceneMain::enemyExplode(Enemy* enemy)
{
    // 爆炸特效
    Uint32 curTime = SDL_GetTicks();
    Explosion* explode = new Explosion(explosionTemp);
    explode->position.x = enemy->position.x + enemy->width/2 - explode->width/2;
    explode->position.y = enemy->position.y + enemy->height/2 - explode->height/2;
    explode->startTime = curTime;
    explosions.push_back(explode);
    delete enemy;
}

void SceneMain::playerExplode()
{
    Uint32 curTime = SDL_GetTicks();
    isAlive = false;
    Explosion* explode = new Explosion(explosionTemp);
    explode->position.x = player.position.x + player.width/2 - player.width/2;
    explode->position.y = player.position.y + player.height/2 - player.height/2;
    explode->startTime = curTime;
    explosions.push_back(explode);
}

void SceneMain::spawnLifeItem(Enemy* enemy)
{
    Item* itemLife = new Item(itemLifeTemp);
    itemLife->position.x = enemy->position.x + enemy->width/2 - itemLife->width/2;
    itemLife->position.y = enemy->position.y + enemy->height/2 - itemLife->height/2;
}
