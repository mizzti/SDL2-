#include "Game.h"
#include <iostream>
#include <SDL.h>

int main(int, char **)
{
    Game& game = Game::getInstance();
    game.init();
    game.run();
    return 0;
}