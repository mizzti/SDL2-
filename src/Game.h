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
    static Game& getInstance() // -？这里怎么用引用呢-
    {
        static Game instance;
        return instance;
    }

    void init();
    void run();
    void changeScene(Scene* nextScene);
    void clean();

    void handleEvent(SDL_Event* e);
    void update();
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

    int getWindowHight()
    {
        return windowHeight;
    }

private:
    Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete; // -x 删除拷贝运算符的参数-

    bool isRunning = true;
    Scene* currentScene = nullptr;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int windowWidth = 600;
    int windowHeight = 800;
};

#endif