#pragma once
#include <vector>

#include "vec3.h"

namespace pgame
{

// Tuning: change numbers here, not logic.
inline constexpr float kFirstBreakDuration = 3.0f; // before wave 1
inline constexpr float kBreakDuration = 5.0f;      // between waves
inline constexpr float kSpawnInterval = 1.0f;      // enemies trickle in, one at a time

// What one wave looks like. Wave numbers start at 1.
struct WaveStats
{
    int enemyCount;
    float enemySpeed;
    float abilityChance; // 0..1
    int enemyHealth;
};

WaveStats GetWaveStats(int wave);

// Picks a spawn point at least minDistance from the player.
// random01 is a value in [0, 1): randomness is passed in, like ApplySpread.
// If no point is far enough, returns the farthest one.
Vec3 PickSpawnPoint(const std::vector<Vec3> &points, const Vec3 &player, float minDistance,
                    float random01);

// Decides WHEN to spawn. Game decides WHAT and WHERE.
class WaveDirector
{
public:
    // Call once per frame. Returns true when Game should spawn ONE enemy now.
    bool Update(float dt, int aliveEnemies);

    int GetWave() const { return wave; } // 0 = before the first wave
    WaveStats GetStats() const { return GetWaveStats(wave); }
    bool IsOnBreak() const { return onBreak; }
    float GetBreakTimeLeft() const { return breakTimer; }

private:
    bool TickSpawn(float dt);

    int wave = 0;
    int leftToSpawn = 0;
    float spawnTimer = 0.0f;
    float breakTimer = kFirstBreakDuration;
    bool onBreak = true;
};

} // namespace pgame