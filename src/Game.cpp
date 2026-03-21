#include "Game.h"
#include "SceneMain.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

Game::Game()
{
}

Game::~Game()
{
    if (currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }
    clean();
}

void Game::run()
{
    // 主循环
    while (isRunning)
    {
        auto frameStart = SDL_GetTicks();
        SDL_Event event;
        handleEvent(&event);

        // 更新
        update(deltaTime);

        // 渲染
        render();
        auto frameEnd = SDL_GetTicks();
        auto diff = frameEnd - frameStart;// 单位毫秒
        if (diff < frameTime)
        {
            SDL_Delay(frameTime - diff);
            // deltaTime是浮点数的单位是秒，frameTime单位是毫秒
            deltaTime = frameTime / 1000.0f;
        }
        else
        {
            deltaTime = diff / 1000.0f;
        }
    }
}

void Game::init()
{
    // 计算frameTime每秒内一帧所用时间 [BUG] 目标时间计算公式- 单位毫秒
    frameTime = 1000 / FPS;
    // SDL初始化
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Init Error: %s\n", SDL_GetError());
        // std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl; 上面代码与cout功能类似，SDL_Log是SDL的日志函数
        isRunning = false;
    }

    // 创建窗口
    window = SDL_CreateWindow("SDL2 Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    // 创建失败要打印，并设置isRunning为false，因为方便寻找错误位置
    if (window == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    // 创建渲染器
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        isRunning = false;
    }

    // 初始化IMG [BUG] 初始化函数名和参数写错，初始化位置应在Game中-
    if (IMG_Init(IMG_INIT_PNG) != (IMG_INIT_PNG))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_IMG INIT ERROR: %S\n", IMG_GetError());
        isRunning = false;
    }

    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) != (MIX_INIT_MP3 | MIX_INIT_OGG))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_MIX INIT ERROR: %S\n", Mix_GetError());
        isRunning = false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)\
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_MIX OPEN AUDIO ERROR: %S\n", Mix_GetError());
        isRunning = false;
    }

    // 设置channel数量
    Mix_AllocateChannels(32);
    // 调节音频大小
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    Mix_Volume(-1, MIX_MAX_VOLUME / 4);

    currentScene = new SceneMain();
    currentScene->init();
}

void Game::changeScene(Scene* nextScene)
{
    // 清理当前场景
    if (currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene; // currentScene是指针所以清理后需删除释放内存
    }
    // 切换场景
    currentScene = nextScene;
    // 初始化新场景
    currentScene->init();
}

void Game::clean()
{
    Mix_CloseAudio();
    Mix_Quit();
    // 退出图片
    IMG_Quit();
    // 释放窗口
    SDL_DestroyWindow(window);
    // 释放渲染器
    SDL_DestroyRenderer(renderer);
    // 退出SDL
    SDL_Quit();
}

void Game::handleEvent(SDL_Event* event)
{
    while (SDL_PollEvent(event))
    {
        if (event->type == SDL_QUIT)
        {
            isRunning = false;
        }
        // 处理事件
        currentScene->handleEvent(event);
    }
}

void Game::update(float deltaT)
{
    currentScene->update(deltaT);
}

void Game::render()
{
    // 清空
    SDL_RenderClear(renderer);
    // 渲染
    currentScene->render();
    // 更新显示
    SDL_RenderPresent(renderer);
}
