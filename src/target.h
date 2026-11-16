#pragma once
#include "map.h"
#include "ray.h"

class Target
{
public:
    Target(const Box &hitbox, int maxHealth) : hitbox(hitbox), health(maxHealth) {}

    float RayCast(const pgame::Ray &ray, float maxDistance) const; //-1 if miss
    void TakeDamage(int amount) { health = std::max(0, health - amount); }
    bool IsAlive() const { return health > 0; }
    int GetHealth() const { return health; }
    const Box &GetHitbox() const { return hitbox; }

private:
    Box hitbox;
    int health;
};