#pragma once
#include <algorithm>
#include <cmath>
#include <utility>

#include "box.h"
#include "ray.h"

namespace pgame
{
    inline bool ClipSlab(float origin, float dir, float lo, float hi, float &tMin, float &tMax)
    {
        if (std::abs(dir) < 1e-8f)
            return origin >= lo && origin <= hi; // parallel: inside or never

        float t1 = (lo - origin) / dir;
        float t2 = (hi - origin) / dir;
        if (t1 > t2)
            std::swap(t1, t2);

        tMin = std::max(tMin, t1);
        tMax = std::min(tMax, t2);
        return tMin <= tMax;
    }

    // Distance along the ray to the box, or -1 if it misses
    inline float RayVsBox(const Ray &ray, const Box &box, float maxDistance)
    {
        float tMin = 0.0f;
        float tMax = maxDistance;

        const bool hit =
            ClipSlab(ray.origin.x, ray.direction.x, box.x - box.width * 0.5f, box.x + box.width * 0.5f, tMin, tMax) &&
            ClipSlab(ray.origin.y, ray.direction.y, box.y, box.y + box.height, tMin, tMax) &&
            ClipSlab(ray.origin.z, ray.direction.z, box.z - box.depth * 0.5f, box.z + box.depth * 0.5f, tMin, tMax);

        return hit ? tMin : -1.0f;
    }
} // namespace pgame