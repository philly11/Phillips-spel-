#include "ability.h"

namespace pgame
{

const char *AbilityName(AbilityType type)
{
    switch (type)
    {
    case AbilityType::Camo:
        return "CAMOUFLAGE";
    case AbilityType::SuperSpeed:
        return "SUPER SPEED";
    case AbilityType::Blink:
        return "BLINK";
    case AbilityType::InvertedControls:
        return "INVERTED CONTROLS";
    case AbilityType::Giant:
        return "GIANT";
    }
    return "UNKNOWN";
}

AddResult AbilitySlots::Add(AbilityType type)
{
    // 1. Already have it: reset the timer.
    for (auto &slot : slots)
    {
        if (slot && slot->type == type)
        {
            slot->timeLeft = kAbilityDuration;
            return AddResult::Refreshed;
        }
    }

    // 2. Free slot: use it.
    for (auto &slot : slots)
    {
        if (!slot)
        {
            slot = ActiveAbility{type, kAbilityDuration};
            return AddResult::Added;
        }
    }

    // 3. Full: replace the oldest. All abilities start at the same duration,
    //    so the oldest is simply the one with the least time left.
    std::optional<ActiveAbility> *oldest = &slots[0];
    for (auto &slot : slots)
    {
        if (slot->timeLeft < (*oldest)->timeLeft)
        {
            oldest = &slot;
        }
    }
    *oldest = ActiveAbility{type, kAbilityDuration};
    return AddResult::Replaced;
}

void AbilitySlots::Update(float dt)
{
    for (auto &slot : slots)
    {
        if (!slot)
        {
            continue;
        }
        slot->timeLeft -= dt;
        if (slot->timeLeft <= 0.0f)
        {
            slot.reset();
        }
    }
}

bool AbilitySlots::Has(AbilityType type) const
{
    for (const auto &slot : slots)
    {
        if (slot && slot->type == type)
        {
            return true;
        }
    }
    return false;
}

AbilityEffects AbilitySlots::GetEffects() const
{
    AbilityEffects effects;
    for (const auto &slot : slots)
    {
        if (!slot)
        {
            continue;
        }
        switch (slot->type)
        {
        case AbilityType::Camo:
            effects.camo = true;
            break;
        case AbilityType::SuperSpeed:
            effects.speedMultiplier *= kSuperSpeedMultiplier;
            effects.spreadMultiplier *= kSuperSpeedSpreadMultiplier;
            break;
        case AbilityType::Blink:
            effects.canBlink = true;
            break;
        case AbilityType::InvertedControls:
            effects.invertLook = true;
            break;
        case AbilityType::Giant:
            effects.eyeHeightMultiplier *= kGiantEyeHeightMultiplier;
            break;
        }
    }
    return effects;
}

} // namespace pgame