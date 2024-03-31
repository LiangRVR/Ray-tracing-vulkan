#pragma once
#include "Hittable.h"
#include <glm/glm.hpp>

class Sphere : public Hittable
{
public:
    glm::vec3 Position{0.0f};
    float Radius = 0.5f;

    int MaterialIndex = 0;

    Sphere() = default;

    bool hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const override;

    void ClosestHit(const Ray &ray, HitPayload &payload) const override;

    bool RenderObjectOptions() override;
};
