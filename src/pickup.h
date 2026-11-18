#pragma once
#include "vec3.h"

namespace pgame
{

inline constexpr float kPickupRange = 1.0f;       // horizontal reach, metres
inline constexpr float kPickupHeightRange = 1.0f; // vertical reach
inline constexpr float kPickupRespawnTime = 10.0f;

// A spot that hands out an ability. It doesn't know WHICH one:
// Game rolls that on collect, so it's a real mystery.
class Pickup
{
public:
    explicit Pickup(const Vec3 &position) : position(position) {}

    void Update(float dt);
    bool TryCollect(const Vec3 &playerFeet); // true once, then starts respawning
    bool IsAvailable() const { return respawnTimer <= 0.0f; }
    const Vec3 &GetPosition() const { return position; }

private:
    Vec3 position;             // bottom center
    float respawnTimer = 0.0f; // 0 = available
};

} // namespace pgame