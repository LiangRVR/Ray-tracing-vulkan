#include "HittableList.h"

bool HittableList::hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const
{
    HitPayload temp_rec;
    bool hit_anything = false;
    auto closest_so_far = tMax;

    for (size_t i = 0; i < objects.size(); ++i)
    {
        if (objects[i]->hit(ray, tMin, closest_so_far, temp_rec))
        {
            hit_anything = true;
            closest_so_far = temp_rec.HitDistance;
            payload = temp_rec;
            payload.objectIndex = i;
        }
    }

    return hit_anything;
}

void HittableList::ClosestHit(const Ray &ray, HitPayload &payload) const
{
    objects[payload.objectIndex]->ClosestHit(ray, payload);
}
