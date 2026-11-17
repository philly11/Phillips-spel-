#include "target.h"
#include "ray_math.h"
#include "map.h"
#include <cmath>

float Target::RayCast(const pgame::Ray &ray, float maxDistance) const
{
    return pgame::RayVsBox(ray, hitbox, maxDistance);
}

void Target::Update(float dt, const Vec3 &goal, const Map &map)
{
    if (!IsAlive() || speed <= 0.0f)
        return;

    const float dx = goal.x - hitbox.x;
    const float dz = goal.z - hitbox.z;
    const float dist = std::sqrt(dx * dx + dz * dz);
    if (dist <= kStopDistance)
        return;

    const Vec3 prev{hitbox.x, hitbox.y, hitbox.z};
    const Vec3 next{prev.x + dx / dist * speed * dt, prev.y, prev.z + dz / dist * speed * dt};

    const float radius = std::max(hitbox.width, hitbox.depth) * 0.5f;
    const Vec3 r = map.ResolveMove(prev, next, hitbox.y, radius);
    hitbox.x = r.x;
    hitbox.z = r.z;
}