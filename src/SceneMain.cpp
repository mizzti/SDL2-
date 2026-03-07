#include "SceneMain.h"
#include "Game.h"
// 导入IMG库，-x 库名-
#include <SDL_image.h>
#include <SDL.h>

SceneMain::SceneMain() : game(Game::getInstance())
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::init()
{
    // 初始化player -x -
    player.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/SpaceShip.png");
    // 获取材质的长宽作为player的长宽，注意float到int* -x 设置的是玩家的宽高，不是位置-
    SDL_QueryTexture(player.texture, NULL, NULL, &player.width, &player.hight);
    // -x 玩家位置显示错误的原因在于缩小的对象错了，应该缩小玩家对象的长宽，而不是下方渲染时放置玩家的长方形框-
    player.hight /= 4;
    player.width /= 4;
    // 设置玩家初始坐标 -xx 忘了- -注：这里的发生了低精度到高精度的隐式转换，所以无需显示转换-
    player.position.x = game.getWindowWidth() / 2 - player.width / 2;
    player.position.y = game.getWindowHight() - player.hight;

    // 初始化子弹
    projectilePlayerTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/laser-3.png");
    SDL_QueryTexture(projectilePlayerTemplate.texture, NULL, NULL, &projectilePlayerTemplate.width, &projectilePlayerTemplate.hight);
    projectilePlayerTemplate.width /= 4;
    projectilePlayerTemplate.hight /= 4;

}

void SceneMain::handleEvent(SDL_Event* event)
{
}

void SceneMain::update(float deltaTime)
{
    keyboardControl(deltaTime);
    // 更新子弹
    updatePlayerProjectile(deltaTime);
}

void SceneMain::render()
{
    // 先渲染子弹，渲染会按照顺序叠加渲染（类似图层）
    randerProjectilePlayer();

    // -x 高精度float转换到低精度的int，会发生数据丢失，C++禁止这样的隐式转换，所以要显示转换-
    SDL_Rect rect = {static_cast<int>(player.position.x),
                     static_cast<int>(player.position.y),
                     player.width,
                     player.hight};
    // 渲染图片 -x 最后一个参数是放置的位置-
    SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &rect);
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

    // 释放材质 
    if (SDL_DestroyTexture != nullptr)
    {
        // 释放图片材质
        SDL_DestroyTexture(player.texture);
    }
    if (projectilePlayerTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(projectilePlayerTemplate.texture);
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
    if (keyboardState[SDL_SCANCODE_J])
    {
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
    ProjectilePlayer* projectile = new ProjectilePlayer(projectilePlayerTemplate);
    // 设置子弹位置 -x 子弹位置要在发射子弹时再设置-
    projectile->position.x = player.position.x + player.width / 2 - projectile->width / 2;
    projectile->position.y = player.position.y;
    projectilePlayer.push_back(projectile);
}

void SceneMain::updatePlayerProjectile(float deltaTime)
{
    // 使用迭代器更新子弹
    for (auto it = projectilePlayer.begin(); it != projectilePlayer.end();)
    {
        auto projectile = *it;
        // 更新子弹的发射速度
        projectile->position.y -= projectile->speed * deltaTime;
        // 检查子弹是否超出屏幕

        ++it;
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
