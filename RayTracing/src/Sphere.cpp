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

    // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a); // Second hit distance (currently unused)
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
    glm::vec3 origin = ray.Origin - Position;
    payload.position = origin + ray.Direction * payload.HitDistance;
    payload.normal = glm::normalize(payload.position);

    payload.position += Position;
    payload.materialIndex = MaterialIndex;
}

bool Sphere::RenderObjectOptions()
{
    int optionChanged = 0;
    ImGui::Text("Sphere");
    optionChanged += ImGui::SliderFloat3("Position", &Position.x, -10.0f, 10.0f, "%.1f");
    optionChanged += ImGui::SliderFloat("Radius", &Radius, 0.0f, 10.0f);
    optionChanged += ImGui::SliderInt("Material Index", &MaterialIndex, 0, 2);

    return optionChanged > 0;
}
