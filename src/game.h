#pragma once
#include "raylib.h"

class Game
{
public:
    void Run();

private:
    Camera3D camera = {0};

    void Update();
    void Draw();
};