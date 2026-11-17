#pragma once
#include "vec3.h"

namespace pgame
{
    // u, v in [0, 1). spreadRadians is the maximum angle off the aim direction.
    Vec3 ApplySpread(const Vec3 &dir, float spreadRadians, float u, float v);
}