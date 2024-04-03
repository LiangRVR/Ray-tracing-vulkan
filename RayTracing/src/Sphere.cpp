#include "Sphere.h"
#include <imgui.h>

bool Sphere::hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const
{
    glm::vec3 origin = ray.Origin - Position;

    float a = glm::dot(ray.Direction, ray.Direction);
    float b = 2.0f * glm::dot(origin, ray.Direction);
    float c = glm::dot(origin, origin) - Radius * Radius;

    // Quadratic formula discriminant:
    // b^2 - 4ac

    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f)
        return false;

    // Quadratic formula:
    // (-b +- sqrt(discriminant)) / 2a

    float t = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    if (t <= tMin || tMax <= t)
    {
        t = (-b + glm::sqrt(discriminant)) / (2.0f * a);
        if (t <= tMin || tMax <= t)
            return false;
    }

    payload.HitDistance = t;
    return true;
}

void Sphere::ClosestHit(const Ray &ray, HitPayload &payload) const
{
    /* glm::vec3 origin = ray.Origin - Position;
    payload.position = origin + ray.Direction * payload.HitDistance;
    payload.normal = glm::normalize(payload.position);

    payload.position += Position; */
    payload.position = ray.Origin + ray.Direction * payload.HitDistance;
    glm::vec3 outwardNormal = (payload.position - Position) / Radius;
    payload.setFaceNormal(ray, outwardNormal);

    payload.materialIndex = MaterialIndex;
}

bool Sphere::RenderObjectOptions(std::vector<std::string> &materialNames)
{
    int optionChanged = 0;
    optionChanged += ImGui::DragFloat3("Position", &Position.x, 0.1f);
    optionChanged += ImGui::DragFloat("Radius", &Radius, 0.1f, 0.0f, 1000.0f);
    std::string combo_preview_value = materialNames.at(MaterialIndex);
    if (ImGui::BeginCombo("Material", combo_preview_value.c_str()))
    {
        for (int n = 0; n < materialNames.size(); n++)
        {
            const bool is_selected = (MaterialIndex == n);
            if (ImGui::Selectable(materialNames.at(n).c_str(), is_selected))
            {
                MaterialIndex = n;
                optionChanged += 1;
            }


            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }


    return optionChanged > 0;
}
