#pragma once

#include "HittableList.h"
#include "Material.h"

#include <glm/glm.hpp>
#include <vector>


struct Scene
{
    HittableList Hittables;
    
    std::vector<shared_ptr<Hittable>> objects;

    std::vector<shared_ptr<Material>> Materials;

    glm::vec3 SkyColor{0.6f, 0.7f, 0.9f};
};
