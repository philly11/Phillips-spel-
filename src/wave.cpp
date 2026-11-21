#include "wave.h"

#include <algorithm>
#include <cstddef>

namespace pgame
{

WaveStats GetWaveStats(int wave)
{
    const int n = std::max(0, wave - 1); // waves completed before this one
    WaveStats stats;
    stats.enemyCount = 3 + 2 * n;
    stats.enemySpeed = std::min(4.0f, 2.0f + 0.25f * n);   // stays below player speed (5)
    stats.abilityChance = std::min(1.0f, 0.5f + 0.1f * n); // 50% at wave 1, 100% from wave 6
    stats.enemyHealth = 3 + n;
    return stats;
}

Vec3 PickSpawnPoint(const std::vector<Vec3> &points, const Vec3 &player, float minDistance,
                    float random01)
{
    std::vector<Vec3> farEnough;
    std::size_t farthest = 0;
    float farthestDistSq = -1.0f;

    for (std::size_t i = 0; i < points.size(); ++i)
    {
        const float dx = points[i].x - player.x;
        const float dz = points[i].z - player.z;
        const float distSq = dx * dx + dz * dz;

        if (distSq >= minDistance * minDistance)
            farEnough.push_back(points[i]);
        if (distSq > farthestDistSq)
        {
            farthestDistSq = distSq;
            farthest = i;
        }
    }

    if (farEnough.empty())
        return points[farthest];

    // min() guards against random01 == 1.0 giving an index one past the end
    const std::size_t index =
        std::min(farEnough.size() - 1, static_cast<std::size_t>(random01 * farEnough.size()));
    return farEnough[index];
}

bool WaveDirector::Update(float dt, int aliveEnemies)
{
    // Still spawning this wave's enemies
    if (leftToSpawn > 0)
        return TickSpawn(dt);

    // Everyone has spawned; wait until they're all dead
    if (aliveEnemies > 0)
    {
        onBreak = false;
        return false;
    }

    // Wave cleared (or game just started): count down the break
    onBreak = true;
    breakTimer -= dt;
    if (breakTimer > 0.0f)
        return false;

    // Start the next wave
    ++wave;
    leftToSpawn = GetWaveStats(wave).enemyCount;
    spawnTimer = 0.0f; // first enemy right away
    breakTimer = kBreakDuration;
    onBreak = false;
    return TickSpawn(dt);
}

bool WaveDirector::TickSpawn(float dt)
{
    spawnTimer -= dt;
    if (spawnTimer > 0.0f)
        return false;

    --leftToSpawn;
    spawnTimer = kSpawnInterval;
    return true;
}

} // namespace pgame