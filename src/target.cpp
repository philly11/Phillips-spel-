#include "target.h"
#include "ray_math.h"
#include "map.h"
#include <cmath>
#include <algorithm>

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

bool Target::TryAttack(float dt, const Vec3 &targetPosition)
{
    if (!IsAlive())
    {
        return false;
    }

    if (attackTimer > 0.0f)
    {
        attackTimer -= dt;
    }

    const float dx = targetPosition.x - hitbox.x;
    const float dz = targetPosition.z - hitbox.z;
    const float distanceSquared = dx * dx + dz * dz;
    const bool inRange = distanceSquared <= kAttackRange * kAttackRange;

    if (!inRange || attackTimer > 0.0f)
    {
        return false;
    }

    attackTimer = kAttackInterval;
    return true;
}
