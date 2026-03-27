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

    void renderPhase1();
    void renderPhase2();
};

#endif