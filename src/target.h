#pragma once
#include "map.h"
#include "ray.h"
#include "vec3.h"

class Map;

class Target
{
public:
    Target(const Box &hitbox, int maxHealth, float speed = 0.0f)
        : hitbox(hitbox), health(maxHealth), speed(speed) {}

    // Walks toward goal (x/z only), sliding along walls. Does nothing if dead or speed is 0.
    void Update(float dt, const Vec3 &goal, const Map &map);

    float RayCast(const pgame::Ray &ray, float maxDistance) const; //-1 if miss
    void TakeDamage(int amount) { health = std::max(0, health - amount); }
    bool IsAlive() const { return health > 0; }
    int GetHealth() const { return health; }
    const Box &GetHitbox() const { return hitbox; }

private:
    static constexpr float kStopDistance = 1.5f; // stops this far from the goal

    Box hitbox;
    int health;
    float speed;
};