#pragma once
#include <optional>

#include "ability_type.h"
#include "vec3.h"

namespace pgame
{

inline constexpr float kPickupRange = 1.0f;       // horizontal reach, metres
inline constexpr float kPickupHeightRange = 1.0f; // vertical reach
inline constexpr float kPickupRespawnTime = 10.0f;

// Two kinds of pickup:
// - Floor spot: no fixed type (Game rolls a mystery ability), respawns.
// - Enemy drop: holds the dead enemy's ability, collected once, then removed.
class Pickup
{
public:
    explicit Pickup(const Vec3 &position) : position(position) {}
    Pickup(const Vec3 &position, AbilityType dropType) : position(position), fixedType(dropType) {}

    void Update(float dt);
    bool TryCollect(const Vec3 &playerFeet); // true once, then starts respawning
    bool IsAvailable() const { return respawnTimer <= 0.0f; }
    bool IsDrop() const { return fixedType.has_value(); }
    bool IsUsedUp() const { return IsDrop() && !IsAvailable(); } // collected drop: remove it
    std::optional<AbilityType> GetFixedType() const { return fixedType; }
    const Vec3 &GetPosition() const { return position; }

private:
    Vec3 position;                        // bottom center
    std::optional<AbilityType> fixedType; // empty = mystery
    float respawnTimer = 0.0f;            // 0 = available
};

} // namespace pgame