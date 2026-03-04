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
}

void SceneMain::handleEvent(SDL_Event* event)
{
}

void SceneMain::update()
{
    keyboardControl();
}

void SceneMain::render()
{
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
    if (SDL_DestroyTexture != nullptr)
    {
        // 释放图片材质
        SDL_DestroyTexture(player.texture);
    }
}

void SceneMain::keyboardControl()
{
    auto keyboardState = SDL_GetKeyboardState(NULL);
    // 读取键盘状态，设置速度
    if (keyboardState[SDL_SCANCODE_W])
    {
        // 设置速度
        player.position.y -= 1;
    }
    if (keyboardState[SDL_SCANCODE_S])
    {
        player.position.y += 1;
    }
    if (keyboardState[SDL_SCANCODE_A])
    {
        player.position.x -= 1;
    }
    if (keyboardState[SDL_SCANCODE_D])
    {
        player.position.x += 1;
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
}
