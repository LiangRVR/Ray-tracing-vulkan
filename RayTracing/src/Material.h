#pragma once
#include "Ray.h"
#include "Hittable.h"
#include "imgui.h"
#include "string"

#include "Walnut/Random.h"

class Material
{
public:
    virtual ~Material() = default;
    virtual bool scatter(Ray rayIn, HitPayload &payload, glm::vec3 &attenuation, glm::vec3 &scatteredDirection) const = 0;
    virtual bool RenderObjectOptions() { return false; }
    std::string Name;
};

class Lambertian : public Material
{
public:
    Lambertian(const std::string &name) : Name(name){};

    virtual bool scatter(Ray rayIn, HitPayload &payload, glm::vec3 &attenuation, glm::vec3 &scatteredDirection) const override
    {

        glm::vec3 scatteredDirectionTemp = payload.normal + Roughness * Walnut::Random::UnitVector();
        if (glm::all(glm::lessThan(glm::abs(scatteredDirectionTemp), glm::vec3(1e-8))))
        {
            scatteredDirectionTemp = payload.normal;
        }
        scatteredDirection = scatteredDirectionTemp;

        attenuation = Albedo;
        return true;
    }

    bool RenderObjectOptions() override
    {
        int optionChanged = 0;
        optionChanged += ImGui::ColorEdit3("Albedo", &Albedo.x);
        optionChanged += ImGui::SliderFloat("Roughness", &Roughness, 0.0f, 1.0f);
        return optionChanged > 0;
    }

    glm::vec3 Albedo{1.0f};
    float Roughness = 1.0f;
    std::string Name;
};

class Metal : public Material
{
public:
    Metal(const std::string &name) : Name(name){};

    bool scatter(Ray rayIn, HitPayload &payload, glm::vec3 &attenuation, glm::vec3 &scatteredDirection)
        const override
    {
        glm::vec3 reflected = glm::reflect(glm::normalize(rayIn.Direction), payload.normal);
        glm::vec3 scatteredDirectionTemp = reflected + Fuzz * Walnut::Random::InUnitSphere();
        scatteredDirection = scatteredDirectionTemp;
        attenuation = Albedo;
        return glm::dot(scatteredDirection, payload.normal) > 0;
    }

    bool RenderObjectOptions() override
    {
        int optionChanged = 0;
        optionChanged += ImGui::ColorEdit3("Albedo", &Albedo.x);
        optionChanged += ImGui::SliderFloat("Fuzz", &Fuzz, 0.0f, 1.0f);
        return optionChanged > 0;
    }
    glm::vec3 Albedo{1.0f};
    float Fuzz = 1.0f;
    std::string Name;
};
