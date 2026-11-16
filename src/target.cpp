#include "target.h"
#include "ray_math.h"

float Target::RayCast(const pgame::Ray &ray, float maxDistance) const
{
    return pgame::RayVsBox(ray, hitbox, maxDistance);
}