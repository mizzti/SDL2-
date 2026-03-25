#ifndef SCENETITLE_H
#define SCENETITLE_H

#include "Scene.h"
#include <SDL.h>
#include <SDL_mixer.h>

class SceneTitle : public Scene
{
public:
    void init() override;
    void handleEvent(SDL_Event* event) override;
    void update(float deltaTime) override;
    void render() override;
    void clean() override;

private:
    Mix_Music* bgm;
};

#endif