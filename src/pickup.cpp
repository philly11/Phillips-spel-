#include "pickup.h"

#include <algorithm>
#include <cmath>

namespace pgame
{

void Pickup::Update(float dt)
{
    respawnTimer = std::max(0.0f, respawnTimer - dt);
}

bool Pickup::TryCollect(const Vec3 &playerFeet)
{
    if (!IsAvailable())
        return false;

    const float dx = playerFeet.x - position.x;
    const float dz = playerFeet.z - position.z;
    const bool closeXZ = dx * dx + dz * dz <= kPickupRange * kPickupRange;
    const bool closeY = std::abs(playerFeet.y - position.y) <= kPickupHeightRange;
    if (!closeXZ || !closeY)
        return false;

    respawnTimer = kPickupRespawnTime;
    return true;
}

} // namespace pgame