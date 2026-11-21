#pragma once
#include <optional>

#include "ability_type.h"
#include "box.h"
#include "map.h"
#include "ray.h"
#include "vec3.h"

class Target
{
public:
    static constexpr float kStopDistance = 1.5f;
    static constexpr float kAttackRange = 2.0f;    // horizontal (x/z) distance
    static constexpr float kAttackInterval = 1.0f; // seconds between attacks
    static constexpr int kAttackDamage = 10;

    // Enemy ability tuning. Separate from the player's on purpose:
    // what's fair for an enemy isn't always what's fun for the player.
    static constexpr float kSpeedMultiplier = 1.6f;
    static constexpr float kGiantHeightMultiplier = 1.5f;
    static constexpr int kGiantDamageMultiplier = 2;
    static constexpr float kBlinkDistance = 4.0f;
    static constexpr float kBlinkMinDistance = 4.0f; // only blink when at least this far away
    static constexpr float kBlinkCooldown = 3.0f;
    static constexpr float kCamoDelay = 0.5f; // seconds still before invisible

    static_assert(kAttackRange > kStopDistance, "Enemy would stop before reaching attack range");

    Target(const Box &startHitbox, int maxHealth, float startSpeed = 0.0f,
           std::optional<pgame::AbilityType> abilityType = std::nullopt);

    // Walks toward goal (x/z only), sliding along walls. Blink enemies sometimes teleport.
    void Update(float dt, const Vec3 &goal, const Map &map);
    bool TryAttack(float dt, const Vec3 &targetPosition);

    float RayCast(const pgame::Ray &ray, float maxDistance) const; //-1 if miss
    void TakeDamage(int amount) { health = std::max(0, health - amount); }

    bool IsAlive() const { return health > 0; }
    bool IsVisible() const; // false while a camo enemy stands still
    int GetHealth() const { return health; }
    int GetMaxHealth() const { return maxHealth; }
    int GetAttackDamage() const;
    std::optional<pgame::AbilityType> GetAbility() const { return ability; }
    const Box &GetHitbox() const { return hitbox; }

private:
    void MoveToward(float dt, const Vec3 &goal, const Map &map);
    void Blink(float dirX, float dirZ, float distanceToGoal, const Map &map);
    bool Has(pgame::AbilityType type) const { return ability == type; }
    float GetRadius() const;

    Box hitbox;
    int maxHealth;
    int health;
    float speed;
    std::optional<pgame::AbilityType> ability; // empty = plain enemy
    float attackTimer = 0.0f;                  // 0 = ready, so the first attack is immediate
    float blinkTimer = 0.0f;
    float stillTimer = 0.0f;
};