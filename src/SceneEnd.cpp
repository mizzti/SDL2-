#include "SceneEnd.h"
#include "Game.h"
#include <string>
#include <SDL.h>

void SceneEnd::init()
{
    if (!SDL_IsTextInputActive())
    {
        SDL_StartTextInput();
    }
    if (SDL_IsTextInputActive() == false)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to start text input: %s\n", SDL_GetError());
    }
}

void SceneEnd::handleEvent(SDL_Event *event)
{
    if (isTypeing)
    {
        if (event->type == SDL_TEXTINPUT)
        {
            name += event->text.text;
        }
        if (event->type == SDL_KEYDOWN)
        {
            if (event->key.keysym.scancode == SDL_SCANCODE_RETURN)
            {
                isTypeing = false;
                SDL_StopTextInput();
            }
            if (event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
            {
                // 判断不同字符进行不同的删除操作
                deleteCh(name);
            }
        }
    }
    else
    {

    }
}

void SceneEnd::update(float)
{
}

void SceneEnd::render()
{
    if (isTypeing)
    {
        renderPhase1();
    }
    else
    {
        renderPhase2();
    }
}

void SceneEnd::clean()
{
}

void SceneEnd::renderPhase1()
{
    int score = game.getFinalScore();
    std::string text1 = "你的的分是：" + std::to_string(score);
    game.renderTextCenter(text1, 0.2f, false);

    std::string text2 = "Game Over";
    game.renderTextCenter(text2, 0.4f, true);

    std::string text3 = "请输入你的名字，按回车键确认：";
    game.renderTextCenter(text3, 0.6f, false);

    if (name != "")
    {
        game.renderTextCenter(name, 0.7f, false);
    }
}

void SceneEnd::renderPhase2()
{
}

void SceneEnd::deleteCh(std::string& str)
{
    if (str.empty()) return;
    char lastCh = str.back();// 只赋值了一次，对空字符串pop在MSVC中会触发断言
    if ((lastCh & 0b10000000) == 0b10000000)
    {
        // 中文后续
        str.pop_back();
        while ((lastCh & 0b11000000) != 0b11000000)// 中文头部
        {
            str.pop_back();
            if (str.empty()) return;
            lastCh = str.back();// 继续获取字符串的最后一个字符
        }
    }
    str.pop_back();
}
