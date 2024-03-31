#pragma once

#include "Ray.h"

struct HitPayload
{
    float HitDistance; // t value
    glm::vec3 position;
    glm::vec3 normal;

    int objectIndex;
    int materialIndex;
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const = 0;

    virtual void ClosestHit(const Ray &ray, HitPayload &payload) const = 0;

    virtual bool RenderObjectOptions() { return false; }
};
