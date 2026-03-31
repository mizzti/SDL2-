#include "Game.h"
#include "SceneMain.h"
#include "SceneTitle.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <fstream>

Game::Game()
{
}

Game::~Game()
{
    saveData();
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
    deltaTime = frameTime / 1000.0f;// 防止第一帧出现的随机值

    // SDL初始化
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Init Error: %s\n", SDL_GetError());
        // std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl; 上面代码与cout功能类似，SDL_Log是SDL的日志函数
        isRunning = false;
    }

    // 强制显示系统输入法候选框（Windows/Linux/macOS通用）
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");// 需要在创建窗口前设置

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

    // 初始化音频
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) != (MIX_INIT_MP3 | MIX_INIT_OGG))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_MIX INIT ERROR: %S\n", Mix_GetError());
        isRunning = false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_MIX OPEN AUDIO ERROR: %S\n", Mix_GetError());
        isRunning = false;
    }

    // 设置channel数量
    Mix_AllocateChannels(32);
    // 调节音频大小
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    Mix_Volume(-1, MIX_MAX_VOLUME / 4);

    // 初始化字体
    if (TTF_Init() < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_TTF INIT ERROR: %S\n", TTF_GetError());/// 使用SDL_GetError是一样的
        isRunning = false;
    }

    // 初始化卷轴背景
    nearStars.texture = IMG_LoadTexture(getRenderer(), "assets/image/Stars-A.png");
    SDL_QueryTexture(nearStars.texture, NULL, NULL, &nearStars.width, &nearStars.height);
    nearStars.width /= 2;
    nearStars.height /= 2;

    farStars.texture = IMG_LoadTexture(getRenderer(), "assets/image/Stars-B.png");
    SDL_QueryTexture(farStars.texture, NULL, NULL, &farStars.width, &farStars.height);
    farStars.width /= 2;
    farStars.height /= 2;
    if (nearStars.texture == nullptr || farStars.texture == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load background textures: %s\n", IMG_GetError());
        isRunning = false;
    }

    // 载入字体
    titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 64);
    textFont = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 32);
    if (titleFont == nullptr || textFont == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s\n", TTF_GetError());
        isRunning = false;
    }

    loadData();

    currentScene = new SceneTitle();
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
    if (currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }

    if (nearStars.texture != nullptr)
    {
        SDL_DestroyTexture(nearStars.texture);
        nearStars.texture = nullptr;
    }
    if (farStars.texture != nullptr)
    {
        SDL_DestroyTexture(farStars.texture);
        farStars.texture = nullptr;
    }
    
    if (titleFont != nullptr)
    {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (textFont != nullptr)
    {
        TTF_CloseFont(textFont);
        textFont = nullptr;
    }
    // 退出字体
    TTF_Quit();
    // 退出音频
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
    backgroudUpdate(deltaT);
    currentScene->update(deltaT);
}

void Game::render()
{
    // 清空
    SDL_RenderClear(renderer);
    // 渲染星空背景
    renderBackground();
    // 渲染当前场景
    currentScene->render();
    // 更新显示
    SDL_RenderPresent(renderer);
}

SDL_Point Game::renderTextCenter(std::string text, float posY, bool isTitleFont)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface;
    if (isTitleFont)
    {
        surface = TTF_RenderUTF8_Solid(titleFont, text.c_str(), color);
    }
    else
    {
        surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer(), surface);
    int y = static_cast<int>((getWindowHeight() - surface->h) * posY);
    SDL_Rect rect = {
        getWindowWidth()/2 - surface->w/2, 
        y, 
        surface->w, 
        surface->h
    };
    SDL_RenderCopy(getRenderer(), texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    return {rect.x + rect.w, rect.y};
}

void Game::renderTextPos(std::string text, int posX, int posY, bool isLeft)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer(), surface);
    SDL_Rect rect;
    if (isLeft)
    {
        rect = {posX, posY, surface->w, surface->h};
    }
    else
    {
        rect = {getWindowWidth() - posX - surface->w, posY, surface->w, surface->h};
    }
    SDL_RenderCopy(getRenderer(), texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::insertLeaderBoard(int score, std::string string)
{
    leaderBoard.insert({score, string});
    // 得分榜只保留前八
    if (leaderBoard.size() > 8)
    {
        leaderBoard.erase(--leaderBoard.end());
    }
}

void Game::renderBackground()
{
    for (int poY = static_cast<int>(farStars.yOffset); poY < getWindowHeight(); poY += farStars.height)
    {
        for (int poX = 0; poX < getWindowWidth(); poX += farStars.width)
        {
            SDL_Rect dstRect = {poX, poY, farStars.width, farStars.height};
            SDL_RenderCopy(getRenderer(), farStars.texture, NULL, &dstRect);
        }
    }

    for (int poY = static_cast<int>(nearStars.yOffset); poY < getWindowHeight(); poY += nearStars.height)
    {
        for (int poX = 0; poX < getWindowWidth(); poX += nearStars.width)
        {
            SDL_Rect dstRect = {poX, poY, nearStars.width, nearStars.height};
            SDL_RenderCopy(getRenderer(), nearStars.texture, NULL, &dstRect);
        }
    }
}

void Game::backgroudUpdate(float deltaT)
{
    farStars.yOffset += farStars.speed * deltaT;
    if (farStars.position.y >= 0)
    {
        farStars.yOffset -= farStars.height;
    }

    nearStars.yOffset += nearStars.speed *  deltaT;
    if (nearStars.position.y >= 0)
    {
        nearStars.yOffset -= nearStars.height;
    }
}

void Game::saveData()
{
    std::ofstream file("assets/save.dat");//  相对路径，绝对路径前需加"/"
    if (!file.is_open())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open save file for writing\n");
        return;
    }
    for (const auto& data : leaderBoard)
    {
        file << data.first << data.second << std::endl;
    }
}

void Game::loadData()
{
    std::ifstream file ("assets/save.dat");
    if (!file.is_open())
    {
        SDL_Log("Failed to open save file for loading\n");
        return;
    }
    int score;
    std::string name;
    while (file >> score >> name)
    {
        leaderBoard.insert({score, name});
    }
}
