#pragma once
#include <vector>

#include "map.h"
#include "ray.h"
#include "target.h"

namespace pgame
{
    struct ShotResult
    {
        enum class Kind
        {
            None,
            Wall,
            Target
        };

        Kind kind = Kind::None;
        float distance = -1.0f; // along the ray; -1 if nothing was hit
        int targetIndex = -1;   // only valid when kind == Target
    };

    // Decides what a shot hits first. Pure logic: no raylib, no damage applied.
    ShotResult ResolveShot(const Ray &ray, const Map &map,
                           const std::vector<Target> &targets, float maxDistance);
} // namespace pgame