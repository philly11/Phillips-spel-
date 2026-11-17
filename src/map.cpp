#include "map.h"
#include "box.h"
#include "ray_math.h"
#include <cmath>
#include <algorithm>
#include <utility>

Map::Map()
{
    // Initialize the map with some boxes
    //                 x      y     z      w      h     d
    boxes.push_back({0.0f, 0.0f, 10.25f, 20.5f, 3.0f, 0.5f});  // north wall
    boxes.push_back({0.0f, 0.0f, -10.25f, 20.5f, 3.0f, 0.5f}); // south wall
    boxes.push_back({10.25f, 0.0f, 0.0f, 0.5f, 3.0f, 20.0f});  // east wall
    boxes.push_back({-10.25f, 0.0f, 0.0f, 0.5f, 3.0f, 20.0f}); // west wall

    boxes.push_back({-4.0f, 0.0f, 2.0f, 2.0f, 1.0f, 0.6f}); // cover
    boxes.push_back({4.0f, 0.0f, -2.0f, 2.0f, 1.0f, 0.6f}); // cover
    boxes.push_back({0.0f, 0.0f, 5.0f, 1.0f, 2.5f, 1.0f});  // pillar
    boxes.push_back({6.0f, 0.0f, 6.0f, 4.0f, 1.0f, 4.0f});  // platform
    boxes.push_back({6.0f, 0.0f, 3.5f, 4.0f, 0.25f, 1.0f}); // step toward the platform
}

float Map::GetGroundHeight(float x, float z, float feetY) const
{
    float best = 0.0f;
    for (const Box &box : boxes)
    {
        bool insideX = std::abs(x - box.x) <= box.width * 0.5f;
        bool insideZ = std::abs(z - box.z) <= box.depth * 0.5f;
        float top = box.y + box.height;

        if (insideX && insideZ && top <= feetY + kStepHeight && top > best)
            best = top;
    }
    return best;
}

// How deeply the player circle overlaps the worst wall (0 = not touching any)
float Map::WallOverlap(float x, float z, float feetY, float radius) const
{
    float worst = 0.0f;
    for (const Box &box : boxes)
    {
        const float top = box.y + box.height;
        if (top <= feetY + kStepHeight)
            continue; // low enough to step onto, not a wall

        const float dx = box.width * 0.5f + radius - std::abs(x - box.x);
        const float dz = box.depth * 0.5f + radius - std::abs(z - box.z);
        if (dx > 0.0f && dz > 0.0f)
            worst = std::max(worst, std::min(dx, dz));
    }
    return worst;
}

float Map::RayCast(const pgame::Ray &ray, float maxDistance) const
{
    float nearest = -1.0f;
    for (const Box &box : boxes)
    {
        float tMin = 0.0f;
        float tMax = maxDistance;

        const bool hit =
            pgame::ClipSlab(ray.origin.x, ray.direction.x, box.x - box.width * 0.5f, box.x + box.width * 0.5f, tMin, tMax) &&
            pgame::ClipSlab(ray.origin.y, ray.direction.y, box.y, box.y + box.height, tMin, tMax) &&
            pgame::ClipSlab(ray.origin.z, ray.direction.z, box.z - box.depth * 0.5f, box.z + box.depth * 0.5f, tMin, tMax);

        if (hit && (nearest < 0.0f || tMin < nearest))
            nearest = tMin;
    }
    return nearest;
}

Vec3 Map::ResolveMove(const Vec3 &prev, const Vec3 &now, float feetY, float radius) const
{
    float x = now.x;
    float z = now.z;

    // A move is blocked only if it pushes deeper into a wall than before
    if (WallOverlap(x, prev.z, feetY, radius) > WallOverlap(prev.x, prev.z, feetY, radius))
        x = prev.x;
    if (WallOverlap(x, z, feetY, radius) > WallOverlap(x, prev.z, feetY, radius))
        z = prev.z;

    return {x, now.y, z};
}