// #pragma once //头文件保护
// 下面是头文件保护符
#ifndef GAME_H
#define GAME_H
#include "Scene.h"
#include <SDL.h>

class Game
{
public:
    ~Game();
    static Game& getInstance() // [NOTE] 返回引用确保单例不会被意外删除，调用更直观-
    {
        static Game instance;
        return instance;
    }

    void init();
    void run();
    void changeScene(Scene* nextScene);
    void clean();

    void handleEvent(SDL_Event* e);
    void update(float deltaT);
    void render();

    SDL_Renderer* getRenderer()
    {
        return renderer;
    }

    SDL_Window* getWindow()
    {
        return window;
    }

    int getWindowWidth()
    {
        return windowWidth;
    }

    int getWindowHeight()
    {
        return windowHeight;
    }

private:
    Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete; // [BUG] 删除拷贝运算符的参数-

    bool isRunning = true;
    Scene* currentScene = nullptr;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int windowWidth = 600;
    int windowHeight = 800;

    int FPS = 60;
    Uint32 frameTime;
    float deltaTime;// 渲染循环所需时间
};

#endif