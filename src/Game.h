// #pragma once //头文件保护
// 下面是头文件保护符
#ifndef GAME_H
#define GAME_H

#include "Scene.h"
#include "Object.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <map>

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

    // 渲染工具函数
    SDL_Point renderTextCenter(std::string text, float posY, bool isTitleFont);
    void renderTextPos(std::string text, int posX, int posY, bool isLeft = true);

    // set函数
    void setFinalScore(int score){ finalScore = score; }
    void insertLeaderBoard(int score, std::string string);

    // get函数
    int getFinalScore() { return finalScore; }
    SDL_Renderer* getRenderer(){ return renderer; }
    SDL_Window* getWindow(){ return window; }
    int getWindowWidth(){ return windowWidth; }
    int getWindowHeight(){ return windowHeight; }
    std::multimap<int, std::string, std::greater<int>> getLeaderBoard() { return leaderBoard; }

private:
    Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete; // [BUG] 删除拷贝运算符的参数-
    Background farStars;
    Background nearStars;
    int finalScore = 0;
    std::multimap <int, std::string, std::greater<int>> leaderBoard;// 得分榜，需要从大到小排序

    TTF_Font* titleFont;
    TTF_Font* textFont;

    bool isRunning = true;
    Scene* currentScene = nullptr;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int windowWidth = 600;
    int windowHeight = 800;

    int FPS = 60;
    Uint32 frameTime;
    float deltaTime;// 渲染循环所需时间

    void renderBackground();
    void backgroudUpdate(float deltaT);
    void saveData();
    void loadData();
};

#endif