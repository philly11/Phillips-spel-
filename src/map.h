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
    float GetGroundHeight(float x, float z) const;
    const std::vector<Box> &GetBoxes() const { return boxes; }

private:
    std::vector<Box> boxes;
};