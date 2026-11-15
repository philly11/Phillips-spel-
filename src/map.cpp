#include "map.h"
#include <cmath>
#include <algorithm>

Map::Map()
{
    // Initialize the map with some boxes
    //                 x     y     z     w     h     d
    boxes.push_back({0.0f, 0.3f, 0.0f, 2.0f, 1.0f, 2.0f});
    boxes.push_back({5.0f, 0.3f, 3.0f, 3.0f, 0.5f, 3.0f});
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