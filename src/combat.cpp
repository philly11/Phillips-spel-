#include "combat.h"

namespace pgame
{
    ShotResult ResolveShot(const Ray &ray, const Map &map,
                           const std::vector<Target> &targets, float maxDistance)
    {
        ShotResult result;

        // Nearest living target
        int nearestIndex = -1;
        float nearestDist = -1.0f;
        for (size_t i = 0; i < targets.size(); ++i)
        {
            if (!targets[i].IsAlive())
                continue;

            const float d = targets[i].RayCast(ray, maxDistance);
            if (d >= 0.0f && (nearestDist < 0.0f || d < nearestDist))
            {
                nearestDist = d;
                nearestIndex = static_cast<int>(i);
            }
        }

        const float wallDist = map.RayCast(ray, maxDistance);

        // The target counts only if no wall is in front of it
        if (nearestIndex >= 0 && (wallDist < 0.0f || nearestDist < wallDist))
        {
            result.kind = ShotResult::Kind::Target;
            result.distance = nearestDist;
            result.targetIndex = nearestIndex;
        }
        else if (wallDist >= 0.0f)
        {
            result.kind = ShotResult::Kind::Wall;
            result.distance = wallDist;
        }
        return result;
    }
} // namespace pgame