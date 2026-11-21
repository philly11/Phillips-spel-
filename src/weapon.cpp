#include "weapon.h"

#include <algorithm>

Weapon::Weapon(const WeaponConfig &cfg)
    : config(cfg), ammoInMagazine(cfg.magazineSize), reserveAmmo(cfg.reserveAmmo),
      currentSpread(cfg.baseSpread)
{
}

void Weapon::Update(float dt)
{
    cooldown = std::max(0.0f, cooldown - dt);

    // Spread recovers toward its base value
    currentSpread = std::max(config.baseSpread, currentSpread - config.spreadRecovery * dt);

    if (reloading)
    {
        reloadTimer -= dt;
        if (reloadTimer <= 0.0f)
            FinishReload();
    }
}

bool Weapon::TryFire()
{
    if (reloading || cooldown > 0.0f)
        return false;

    if (ammoInMagazine == 0)
    {
        StartReload(); // auto-reload on an empty click
        return false;
    }

    --ammoInMagazine;
    cooldown = config.fireInterval;

    shotSpread = currentSpread; // this shot uses the spread from BEFORE this bullet
    currentSpread =
        std::min(config.maxSpread, currentSpread + config.spreadPerShot * bloomMultiplier);
    return true;
}

void Weapon::StartReload()
{
    if (reloading || ammoInMagazine == config.magazineSize || reserveAmmo == 0)
        return;

    reloading = true;
    reloadTimer = config.reloadTime;
}

void Weapon::FinishReload()
{
    const int needed = config.magazineSize - ammoInMagazine;
    const int taken = std::min(needed, reserveAmmo);
    ammoInMagazine += taken;
    reserveAmmo -= taken;
    reloading = false;
}

void Weapon::RefillAmmo()
{
    ammoInMagazine = config.magazineSize;
    reserveAmmo = config.reserveAmmo;
    reloading = false;
    reloadTimer = 0.0f;
}