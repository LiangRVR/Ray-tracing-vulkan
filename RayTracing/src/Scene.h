#pragma once

#include "HittableList.h"

#include <glm/glm.hpp>
#include <vector>

struct Material
{
    glm::vec3 Albedo{1.0f};
    float Roughness = 1.0f;
    float Metallic = 0.0f;
    glm::vec3 EmissionColor{0.0f};
    float EmissionPower = 0.0f;

    glm::vec3 GetEmission() const { return EmissionColor * EmissionPower; }
};


struct Scene
{
    HittableList Hittables;
    //std::vector<Sphere> Spheres;
    std::vector<Material> Materials;
};
