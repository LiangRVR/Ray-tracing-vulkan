#include "HittableList.h"

/**
 * @brief Determines if a ray hits any object in the list.
 *
 * This function iterates over all objects in the list and checks if the ray hits each object. If the
 * ray hits an object, the function updates the hit payload with information about the hit, including
 * the distance to the hit point and the index of the object hit.
 *
 * The function only considers hits that occur at a distance greater than tMin and less than the
 * closest hit so far. This allows the function to find the closest hit in the list.
 *
 * @param ray The ray to check for hits.
 * @param tMin The minimum distance at which a hit can occur.
 * @param tMax The maximum distance at which a hit can occur.
 * @param payload Output parameter for information about the hit. Only modified if a hit occurs.
 * @return bool Returns true if the ray hits any object in the list; otherwise, returns false.
 */
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

/**
 * @brief Determines the closest hit point on the object hit by the ray.
 *
 * This function is called after a ray has been determined to hit an object in the list. It calls the
 * `ClosestHit` function of the object that was hit, which calculates the closest hit point on the
 * object and updates the hit payload with information about the hit.
 *
 * The object to call `ClosestHit` on is determined by the `objectIndex` field of the hit payload,
 * which is set by the `hit` function.
 *
 * @param ray The ray that hit the object.
 * @param payload The hit payload containing information about the hit. Modified by the `ClosestHit`
 * function of the object hit.
 */
void HittableList::ClosestHit(const Ray &ray, HitPayload &payload) const
{
    objects[payload.objectIndex]->ClosestHit(ray, payload);
}
