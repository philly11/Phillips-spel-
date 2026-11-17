#pragma once
#include <algorithm>
#include <vector>
#include <utility>
#include "ray.h"
#include "box.h"

class Map
{
public:
    Map();
    explicit Map(std::vector<Box> b) : boxes(std::move(b)) {}

    float GetGroundHeight(float x, float z, float feetY) const;
    float WallOverlap(float x, float z, float feetY, float radius) const;
    float RayCast(const pgame::Ray &ray, float maxDistance) const;
    const std::vector<Box> &GetBoxes() const { return boxes; }
    static constexpr float kStepHeight = 0.3f; // Maximum height the player can step up
    // Returns the position after sliding along walls. y is passed through unchanged.
    Vec3 ResolveMove(const Vec3 &prev, const Vec3 &now, float feetY, float radius) const;

private:
    std::vector<Box> boxes;
};