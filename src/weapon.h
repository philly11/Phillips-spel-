#pragma once

struct WeaponConfig
{
    float fireInterval = 0.15f; // seconds between shots
    int magazineSize = 12;
    int reserveAmmo = 36;
    float reloadTime = 1.5f;
    int damage = 1;
    float baseSpread = 0.005f;   // radians, standing still
    float spreadPerShot = 0.01f; // added by each shot
    float maxSpread = 0.06f;
    float spreadRecovery = 0.1f;   // radians per second
    float recoilKick = 0.02f;      // radians the view kicks up per shot
    float recoilSideways = 0.005f; // max random sideways kick, radians
};

class Weapon
{
public:
    explicit Weapon(const WeaponConfig &config = WeaponConfig{});

    void Update(float dt);
    bool TryFire();     // true if a shot was actually fired
    void StartReload(); // ignored if pointless or already reloading

    int GetAmmoInMagazine() const { return ammoInMagazine; }
    int GetReserveAmmo() const { return reserveAmmo; }
    int GetDamage() const { return config.damage; }
    bool IsReloading() const { return reloading; }
    float GetShotSpread() const { return shotSpread; }
    float GetCurrentSpread() const { return currentSpread; }
    float GetRecoilKick() const { return config.recoilKick; }
    float GetRecoilSideways() const { return config.recoilSideways; }
    void SetBloomMultiplier(float m) { bloomMultiplier = m; } // set by abilities, 1 = normal
private:
    void FinishReload();

    WeaponConfig config;
    int ammoInMagazine;
    int reserveAmmo;
    float cooldown = 0.0f;
    float reloadTimer = 0.0f;
    bool reloading = false;
    float currentSpread;
    float shotSpread = 0.0f;
    float bloomMultiplier = 1.0f;
};