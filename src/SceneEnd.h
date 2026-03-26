#ifndef SCENEEND_H// ifndef是if not define的缩写
#define SCENEEND_H

#include "Scene.h"
#include <SDL.h>

class SceneEnd : public Scene
{
public:
    void init();
    void handleEvent(SDL_Event* event);
    void update(float deltaTime);
    void render();
    void clean();
};

#endif