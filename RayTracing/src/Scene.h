#pragma once

#include "HittableList.h"
#include "Material.h"

#include <glm/glm.hpp>
#include <vector>


struct Scene
{
    HittableList Hittables;
    //std::vector<Sphere> Spheres;
    std::vector<shared_ptr<Hittable>> objects;

    std::vector<shared_ptr<Material>> Materials;
};
