#include "SceneMain.h"
#include "Game.h"
// 导入IMG库，[BUG] 库名-
#include <SDL_image.h>
#include <SDL.h>
#include <random>
#include <string>

SceneMain::SceneMain() : game(Game::getInstance()), random(Random::getInstance())
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::init()
{
    // 读取、播放音频
    bgm = Mix_LoadMUS("assets/music/03_Racing_Through_Asteroids_Loop.ogg");
    if (bgm == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load BGM: %s\n", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);

    // 初始化音效
    sounds["playerShoot"] = Mix_LoadWAV("assets/sound/laser_shoot4.wav");
    sounds["enemyShoot"] = Mix_LoadWAV("assets/sound/xs_laser.wav");
    sounds["playerExplode"] = Mix_LoadWAV("assets/sound/explosion1.wav");
    sounds["enemyExplode"] = Mix_LoadWAV("assets/sound/explosion3.wav");
    sounds["hit"] = Mix_LoadWAV("assets/sound/eff11.wav");
    sounds["getItem"] = Mix_LoadWAV("assets/sound/eff5.wav");

    uiHealth = IMG_LoadTexture(game.getRenderer(), "assets/image/Health UI Black.png");
    scoreFont = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 24);

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
    projectilePlayerTemplate.width /= 3;
    projectilePlayerTemplate.height /= 3;

    // 初始化敌机
    enemyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/insect-2.png");
    SDL_QueryTexture(enemyTemplate.texture, NULL, NULL, &enemyTemplate.width, &enemyTemplate.height);
    enemyTemplate.width /= 4;
    enemyTemplate.height /= 4;

    // 初始化敌机子弹
    projectileEnemyTemp.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bullet-2.png");
    SDL_QueryTexture(projectileEnemyTemp.texture, NULL, NULL, &projectileEnemyTemp.width, &projectileEnemyTemp.height);
    projectileEnemyTemp.width /= 4;
    projectileEnemyTemp.height /= 4;

    // 初始化爆炸
    explosionTemp.texture = IMG_LoadTexture(game.getRenderer(), "assets/effect/explosion.png");
    SDL_QueryTexture(explosionTemp.texture, NULL, NULL, &explosionTemp.width, &explosionTemp.height);
    explosionTemp.totalFrame = explosionTemp.width / explosionTemp.height;
    explosionTemp.width = explosionTemp.height; // 矩形

    // 初始化掉落物品--增加生命值
    itemLifeTemp.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bonus_life.png");
    SDL_QueryTexture(itemLifeTemp.texture, NULL, NULL, &itemLifeTemp.width, &itemLifeTemp.height);
    itemLifeTemp.width /= 3;
    itemLifeTemp.height /= 3;

}

void SceneMain::handleEvent(SDL_Event*)
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
    updateItem(deltaTime);
    // 更新敌机子弹
    updateEnemyProjectiles(deltaTime);
    updateExplosions(deltaTime);
}

void SceneMain::render()
{
    // 先渲染子弹，渲染会按照顺序叠加渲染（类似图层：后渲染会在最上方显示）
    renderProjectilePlayer();
    renderPlayer();
    // 渲染敌机子弹
    renderProjectilesEnemy();
    // 渲染敌机
    renderEnemies();
    renderItem();
    renderExplosions();
    renderUi();
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
    for (auto& item : items)
    {
        if (item != nullptr)
        {
            delete item;
        }
    }
    items.clear();

    for (auto& sound : sounds)
    {
        if (sound.second != nullptr)
        {
            Mix_FreeChunk(sound.second);
        }
    }
    sounds.clear();

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
    if (itemLifeTemp.texture != nullptr)
    {
        SDL_DestroyTexture(itemLifeTemp.texture);
        itemLifeTemp.texture = nullptr;
    }

    // 清理ui血量材质
    if (uiHealth != nullptr)
    {
        SDL_DestroyTexture(uiHealth);
        uiHealth = nullptr;
    }

    // 清理字体
    if (scoreFont != nullptr)
    {
        // 关闭字体
        TTF_CloseFont(scoreFont);
    }

    // 清理音乐
    if (bgm != nullptr)
    {
        // 关闭音乐
        Mix_HaltMusic();
        // 清理
        Mix_FreeMusic(bgm);
    }
}

void SceneMain::updatePlayer(float)
{
    if (!isAlive)
    {
        return;
    }

    if (player.curHealth <= 0)
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
                    Mix_PlayChannel(-1, sounds["hit"], 0);
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
                player.curHealth -= 1;
            }

            if (enemy->health <= 0)
            {
                enemyExplode(enemy);
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
                player.curHealth -= projsEnemy->demage;
                delete projsEnemy;
                it = projectileEnemy.erase(it);
                Mix_PlayChannel(-1, sounds["hit"], 0);
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

void SceneMain::updateItem(float deltaTime)
{
    for (auto it = items.begin(); it != items.end();)
    {
        Item* item = *it;
        // [BUG]应累加位置+=
        item->position.x += item->direction.x * item->speed * deltaTime;
        item->position.y += item->direction.y * item->speed * deltaTime;
        // 边缘反弹 [BUG]先判断是否需要反弹，反弹的位置判断需要注意
        // 检查物品和是否超过屏幕边界
        if ((item->position.x < 0 || item->position.x + item->width > game.getWindowWidth()) 
            && item->bounceCount > 0)
        {
            item->direction.x = - item->direction.x;
            item->bounceCount--;
        }
        if ((item->position.y < 0 || item->position.y + item->height > game.getWindowHeight())
            && item->bounceCount > 0)
        {
            item->direction.y = - item->direction.y;
            item->bounceCount--;
        }
        // 物品超出屏幕后删除
        if (item->position.x + item->width < 0 || 
            item->position.x > game.getWindowWidth() ||
            item->position.y + item->height < 0 ||
            item->position.y > game.getWindowHeight())
        {
            delete item;
            it = items.erase(it);
            // SDL_Log("itemLife被删除");
        }
        else
        {
            // 与玩家的碰撞检测
            SDL_Rect itemRect = {
                static_cast<int>(item->position.x), 
                static_cast<int>(item->position.y), 
                item->width, 
                item->height
            };
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x), 
                static_cast<int>(player.position.y), 
                player.width, 
                player.height
            };
            if (isAlive && SDL_HasIntersection(&itemRect, &playerRect))
            {
                playerGetItem(item);
                delete item;
                it = items.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
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

void SceneMain::renderUi()
{
    // 设置血量ui的大小位置
    int x = 10;
    int y = 10;
    int size = 32; // 32 x 32像素的正方形，是边长
    int offset = 5;
    SDL_SetTextureColorMod(uiHealth, 100, 100, 100);
    for (int i = 0; i < player.maxHealth; ++i)
    {
        SDL_Rect dst = {x + i * (size + offset), y, size, size};
        SDL_RenderCopy(game.getRenderer(), uiHealth, NULL, &dst);
    }
    SDL_SetTextureColorMod(uiHealth, 255, 255, 255);
    for (int i = 0; i < player.curHealth; ++i)
    {
        SDL_Rect dst = {x + i * (size + offset), y, size, size};
        SDL_RenderCopy(game.getRenderer(), uiHealth, NULL, &dst);
    }

    // 设置积分显示，to_string在string库中
    std::string text = "得分:" + std::to_string(player.score);
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderUTF8_Solid(scoreFont, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(game.getRenderer(), surface);
    SDL_Rect dst = { game.getWindowWidth() - surface->w - 10, 10, surface->w, surface->h};
    SDL_RenderCopy(game.getRenderer(), texture, NULL, &dst);
    // 字体每帧都要进行删除
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
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

void SceneMain::renderItem()
{
    for (Item* item : items)
    {
        SDL_Rect itemRect = {
            static_cast<int>(item->position.x),
            static_cast<int>(item->position.y), 
            item->width, 
            item->height
        };
        SDL_RenderCopy(game.getRenderer(), item->texture, NULL, &itemRect);
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
    Mix_PlayChannel(0, sounds["playerShoot"], 0);
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
    Mix_PlayChannel(-1, sounds["enemyShoot"], 0);
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
    Mix_PlayChannel(-1, sounds["enemyExplode"], 0);
    // 设置掉落概率
    if (random.getFloat() < 0.6f)
    {
        dropItem(enemy);
    }
    player.score += 10;
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
    Mix_PlayChannel(-1, sounds["playerExplode"], 0);
}

void SceneMain::dropItem(Enemy* enemy)
{
    Item* itemLife = new Item(itemLifeTemp);
    itemLife->position.x = enemy->position.x + enemy->width/2 - itemLife->width/2;
    itemLife->position.y = enemy->position.y + enemy->height/2 - itemLife->height/2;
    float dir = random.getFloat() * 2 * M_PI;
    itemLife->direction.x = std::cos(dir);
    itemLife->direction.y = std::sin(dir);
    items.push_back(itemLife);
}

void SceneMain::playerGetItem(Item* item)
{
    player.score += 5;
    Mix_PlayChannel(-1, sounds["hit"], 0);
    if (item->type == ItemType::Life && player.curHealth < player.maxHealth)
    {
        player.curHealth += 1;
    }
}
