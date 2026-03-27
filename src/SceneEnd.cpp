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
                name.pop_back();
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
