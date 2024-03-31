#pragma once

#include "Hittable.h"
#include "Ray.h"

#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;

class HittableList : public Hittable
{
public:
    std::vector<shared_ptr<Hittable>> objects;

    HittableList() {}
    HittableList(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<Hittable> object)
    {
        objects.push_back(object);
    }

    bool hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const override;
    void ClosestHit(const Ray &ray, HitPayload &payload) const override;
};
