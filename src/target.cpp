#include "target.h"

#include <algorithm>
#include <cmath>

#include "map.h"
#include "ray_math.h"

Target::Target(const Box &startHitbox, int maxHealth, float startSpeed,
               std::optional<pgame::AbilityType> abilityType)
    : hitbox(startHitbox), maxHealth(maxHealth), health(maxHealth), speed(startSpeed),
      ability(abilityType)
{
    // Permanent abilities are applied once, here
    if (Has(pgame::AbilityType::SuperSpeed))
        speed *= kSpeedMultiplier;
    if (Has(pgame::AbilityType::Giant))
        hitbox.height *= kGiantHeightMultiplier;
}

float Target::RayCast(const pgame::Ray &ray, float maxDistance) const
{
    return pgame::RayVsBox(ray, hitbox, maxDistance);
}

void Target::Update(float dt, const Vec3 &goal, const Map &map)
{
    if (!IsAlive())
        return;

    blinkTimer = std::max(0.0f, blinkTimer - dt);

    const float startX = hitbox.x;
    const float startZ = hitbox.z;
    MoveToward(dt, goal, map);

    const bool moved = hitbox.x != startX || hitbox.z != startZ;
    stillTimer = moved ? 0.0f : stillTimer + dt;
}

void Target::MoveToward(float dt, const Vec3 &goal, const Map &map)
{
    if (speed <= 0.0f)
        return;

    const float dx = goal.x - hitbox.x;
    const float dz = goal.z - hitbox.z;
    const float dist = std::sqrt(dx * dx + dz * dz);
    if (dist <= kStopDistance)
        return;

    const float dirX = dx / dist;
    const float dirZ = dz / dist;

    if (Has(pgame::AbilityType::Blink) && blinkTimer <= 0.0f && dist >= kBlinkMinDistance)
    {
        Blink(dirX, dirZ, dist, map);
        blinkTimer = kBlinkCooldown;
        return;
    }

    const Vec3 prev{hitbox.x, hitbox.y, hitbox.z};
    const Vec3 next{prev.x + dirX * speed * dt, prev.y, prev.z + dirZ * speed * dt};

    const Vec3 r = map.ResolveMove(prev, next, hitbox.y, GetRadius());
    hitbox.x = r.x;
    hitbox.z = r.z;
}

// Same idea as the player's blink: cast just above step height, stop before walls
void Target::Blink(float dirX, float dirZ, float distanceToGoal, const Map &map)
{
    // Never blink past the stop distance, or it would land on top of the player
    const float wanted = std::min(kBlinkDistance, distanceToGoal - kStopDistance);

    const pgame::Ray ray{{hitbox.x, hitbox.y + Map::kStepHeight + 0.01f, hitbox.z},
                         {dirX, 0.0f, dirZ}};
    const float hit = map.RayCast(ray, wanted);
    const float distance = hit >= 0.0f ? std::max(0.0f, hit - GetRadius()) : wanted;

    hitbox.x += dirX * distance;
    hitbox.z += dirZ * distance;
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

bool Target::IsVisible() const
{
    return !Has(pgame::AbilityType::Camo) || stillTimer < kCamoDelay;
}

int Target::GetAttackDamage() const
{
    return Has(pgame::AbilityType::Giant) ? kAttackDamage * kGiantDamageMultiplier : kAttackDamage;
}

float Target::GetRadius() const
{
    return std::max(hitbox.width, hitbox.depth) * 0.5f;
}