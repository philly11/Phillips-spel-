#pragma once
#include <vector>

struct Box
{
    float x, y, z;              // Position of the box
    float width, height, depth; // Dimensions of the box
};

class Map
{
public:
    Map();
    float GetGroundHeight(float x, float z, float feetY) const;
    float WallOverlap(float x, float z, float feetY, float radius) const;
    static constexpr float kStepHeight = 0.5f; // Maximum height the player can step up
    const std::vector<Box> &GetBoxes() const { return boxes; }

private:
    std::vector<Box> boxes;
};