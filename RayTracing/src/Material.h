#pragma once
#include "Ray.h"
#include "Hittable.h"
#include "imgui.h"
#include "string"

#include "Utils.h"

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

        glm::vec3 scatteredDirectionTemp = payload.normal + Roughness * Utils::UnitVector();
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
        // optionChanged += ImGui::SliderFloat("Roughness", &Roughness, 0.0f, 1.0f);
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
        glm::vec3 scatteredDirectionTemp = reflected + Fuzz * Utils::InUnitSphere();
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

class Dielectric : public Material
{
public:
    Dielectric(const std::string &name) : Name(name){};

    bool scatter(Ray rayIn, HitPayload &payload, glm::vec3 &attenuation, glm::vec3 &scatteredDirection)
        const override
    {
        attenuation = glm::vec3(1.0, 1.0, 1.0);
        float refraction_ratio = payload.frontFace ? (1.0 / IndexOfRefraction) : IndexOfRefraction;

        glm::vec3 unit_direction = glm::normalize(rayIn.Direction);
        float cos_theta = fmin(glm::dot(-unit_direction, payload.normal), 1.0);
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        glm::vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > Utils::RandomFloat())
            direction = glm::reflect(unit_direction, payload.normal);
        else
            direction = glm::refract(unit_direction, payload.normal, refraction_ratio);

        scatteredDirection = direction;
        return true;
    }

    bool RenderObjectOptions() override
    {
        int optionChanged = 0;
        optionChanged += ImGui::SliderFloat("IOR", &IndexOfRefraction, 1.0f, 3.0f);
        return optionChanged > 0;
    }

    std::string Name;
    float IndexOfRefraction = 1.5f;
    private:
        static float reflectance(float cosine, float ref_idx)
        {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0 * r0;
            return r0 + (1 - r0) * pow((1 - cosine), 5);
        }
};
