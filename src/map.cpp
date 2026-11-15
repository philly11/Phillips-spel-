#include "map.h"
#include <cmath>

Map::Map()
{
    // Initialize the map with some boxes
    //                 x     y     z     w     h     d
    boxes.push_back({0.0f, 0.3f, 0.0f, 2.0f, 1.0f, 2.0f});
    boxes.push_back({5.0f, 0.3f, 3.0f, 3.0f, 0.5f, 3.0f});
}

float Map::GetGroundHeight(float x, float z) const
{
    float best = 0.0f;
    for (const Box &box : boxes)
    {
        bool insideX = std::abs(x - box.x) <= box.width * 0.5f;
        bool insideZ = std::abs(z - box.z) <= box.depth * 0.5f;
        float top = box.y + box.height;

        if (insideX && insideZ && top > best)
            best = top;
    }
    return best;
}
