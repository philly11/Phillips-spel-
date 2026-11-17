#include "spread.h"
#include <cmath>

namespace pgame
{
    static Vec3 Cross(const Vec3 &a, const Vec3 &b)
    {
        return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    }

    static Vec3 Normalize(const Vec3 &a)
    {
        const float len = std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
        return {a.x / len, a.y / len, a.z / len};
    }

    Vec3 ApplySpread(const Vec3 &dir, float spreadRadians, float u, float v)
    {
        // Two axes perpendicular to dir
        const Vec3 ref = std::abs(dir.y) > 0.99f ? Vec3{1, 0, 0} : Vec3{0, 1, 0};
        const Vec3 right = Normalize(Cross(dir, ref));
        const Vec3 up = Cross(right, dir);

        // Uniform point on a disc: sqrt keeps it from clustering in the centre
        const float angle = u * 6.2831853f;
        const float radius = std::sqrt(v) * std::tan(spreadRadians);

        return Normalize({dir.x + (right.x * std::cos(angle) + up.x * std::sin(angle)) * radius,
                          dir.y + (right.y * std::cos(angle) + up.y * std::sin(angle)) * radius,
                          dir.z + (right.z * std::cos(angle) + up.z * std::sin(angle)) * radius});
    }
} // namespace pgame