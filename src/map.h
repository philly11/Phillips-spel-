#pragma once
#include <algorithm>
#include <vector>
#include <utility>
struct Box
{
    float x, y, z;              // Position of the box
    float width, height, depth; // Dimensions of the box
};

class Map
{
public:
    Map();
    explicit Map(std::vector<Box> b) : boxes(std::move(b)) {}

    float GetGroundHeight(float x, float z, float feetY) const;
    float WallOverlap(float x, float z, float feetY, float radius) const;
    const std::vector<Box> &GetBoxes() const { return boxes; }
    static constexpr float kStepHeight = 0.3f; // Maximum height the player can step up

private:
    std::vector<Box> boxes;
};