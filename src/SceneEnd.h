#ifndef SCENEEND_H// ifndef是if not define的缩写
#define SCENEEND_H

#include "Scene.h"
#include <SDL.h>
#include <string>

class SceneEnd : public Scene
{
public:
    void init();
    void handleEvent(SDL_Event* event);
    void update(float deltaTime);
    void render();
    void clean();

private:
    bool isTypeing = true;
    std::string name;
    float blinkTimer = 1.0f;// 闪烁时间

    void renderPhase1();
    void renderPhase2();
    void deleteCh(std::string& str);// 需要对原字符串进行修改
};

#endif