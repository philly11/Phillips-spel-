#pragma once

#include <array>
#include <optional>

namespace pgame
{

enum class AbilityType
{
    Camo,
    SuperSpeed,
    Blink,
    InvertedControls,
    Giant
};

inline constexpr int kAbilityTypeCount = 5; // keep in sync with AbilityType

enum class AddResult
{
    Added,
    Refreshed,
    Replaced
};

// Tuning values: change numbers here, not logic.
inline constexpr float kAbilityDuration = 60.0f;
inline constexpr float kSuperSpeedMultiplier = 1.6f;
inline constexpr float kSuperSpeedSpreadMultiplier = 2.0f;
inline constexpr float kGiantEyeHeightMultiplier = 1.5f;

struct ActiveAbility
{
    AbilityType type;
    float timeLeft;
};

// The "decision": what the active abilities mean for this frame.
// Defaults are neutral, so no abilities = no change.
struct AbilityEffects
{
    float speedMultiplier = 1.0f;
    float spreadMultiplier = 1.0f;
    float eyeHeightMultiplier = 1.0f;
    bool invertLook = false;
    bool camo = false;
    bool canBlink = false;
};

const char *AbilityName(AbilityType type);

class AbilitySlots
{
public:
    static constexpr int kSlotCount = 2;
    using Slots = std::array<std::optional<ActiveAbility>, kSlotCount>;

    AddResult Add(AbilityType type);
    void Update(float dt);
    bool Has(AbilityType type) const;
    AbilityEffects GetEffects() const;
    const Slots &GetSlots() const { return slots; }

private:
    Slots slots{};
};

} // namespace pgame