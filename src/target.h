#pragma once
#include "box.h"
#include "map.h"
#include "ray.h"
#include "vec3.h"

class Map;

class Target
{
public:
    static constexpr float kStopDistance = 1.5f;
    static constexpr float kAttackRange = 2.0f;    // horizontal (x/z) distance
    static constexpr float kAttackInterval = 1.0f; // seconds between attacks
    static constexpr int kAttackDamage = 10;

    static_assert(kAttackRange > kStopDistance,
                  "Enemy would stop before reaching attack range");

    Target(const Box &hitbox, int maxHealth, float speed = 0.0f)
        : hitbox(hitbox), health(maxHealth), speed(speed) {}

    // Walks toward goal (x/z only), sliding along walls. Does nothing if dead or speed is 0.
    void Update(float dt, const Vec3 &goal, const Map &map);
    bool TryAttack(float dt, const Vec3 &targetPosition);

    float RayCast(const pgame::Ray &ray, float maxDistance) const; //-1 if miss
    void TakeDamage(int amount) { health = std::max(0, health - amount); }

    bool IsAlive() const { return health > 0; }
    int GetHealth() const { return health; }
    const Box &GetHitbox() const { return hitbox; }

private:
    Box hitbox;
    int health;
    float speed;
    float attackTimer = 0.0f; // 0 = ready, so the first attack is immediate
};