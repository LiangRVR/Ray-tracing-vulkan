#pragma once

#include "Ray.h"
#include <vector>
#include <string>

struct HitPayload
{
    float HitDistance; // t value
    bool frontFace;
    glm::vec3 position;
    glm::vec3 normal;

    int objectIndex;
    int materialIndex;

    void setFaceNormal(const Ray &r, const glm::vec3 &outward_normal)
    {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        frontFace = glm::dot(r.Direction, outward_normal) < 0;
        normal = frontFace ? outward_normal : -outward_normal;
    }
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const = 0;

    virtual void ClosestHit(const Ray &ray, HitPayload &payload) const = 0;

    virtual bool RenderObjectOptions(std::vector<std::string> &materialNames) { return false; }
    virtual void setMaterialIndex(int newMaterialIndex) {}
    virtual int getMaterialIndex() { return -1; }
};
