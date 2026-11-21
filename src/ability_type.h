#pragma once

namespace pgame
{

// Shared by the player's ability slots, enemies and pickups.
enum class AbilityType
{
    Camo,
    SuperSpeed,
    Blink,
    InvertedControls,
    Giant
};

inline constexpr int kAbilityTypeCount = 5; // keep in sync with AbilityType

const char *AbilityName(AbilityType type); // defined in ability.cpp

} // namespace pgame