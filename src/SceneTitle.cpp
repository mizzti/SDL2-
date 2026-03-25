#include "SceneTitle.h"
#include "Game.h"
#include <string>

void SceneTitle::init()
{
    bgm = Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    if (bgm == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load BGM: %s\n", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);
}

void SceneTitle::handleEvent(SDL_Event *event)
{
}

void SceneTitle::update(float deltaTime)
{
}

void SceneTitle::render()
{
    // 渲染标题
    std::string title = "SDL太空大战";
    game.renderTextCenter(title, 0.3, true);
    // 渲染普通文字
    std::string text = "点击J进入游戏";
    game.renderTextCenter(text, 0.8, false);
}

void SceneTitle::clean()
{
}
