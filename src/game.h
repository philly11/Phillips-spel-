#pragma once
#include "raylib.h"
#include "player.h"

class Game
{
public:
    void Run();

private:
    Player player;
    Camera3D camera = {0};

    void Update();
    void Draw();
};
