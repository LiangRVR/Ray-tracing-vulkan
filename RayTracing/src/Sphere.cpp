#include "Sphere.h"
#include <imgui.h>

/**
 * @brief Determines if a ray hits the sphere.
 *
 * This function checks if a ray hits the sphere by solving the quadratic equation for the intersection
 * of the ray and the sphere. The equation is derived from the geometric definition of a sphere and the
 * parametric equation of a ray.
 *
 * The function first calculates the coefficients of the quadratic equation (a, b, and c) based on the
 * sphere's position and radius and the ray's origin and direction. It then calculates the discriminant
 * of the equation (b^2 - 4ac).
 *
 * If the discriminant is negative, the ray does not hit the sphere. If the discriminant is zero or
 * positive, the ray hits the sphere at one or two points. The function calculates the distance to the
 * closest hit point (t) using the quadratic formula (-b +- sqrt(discriminant)) / 2a.
 *
 * The function only considers hits that occur at a distance greater than tMin and less than tMax. This
 * allows the function to ignore hits that are too close or too far.
 *
 * If a hit occurs, the function updates the hit payload with the distance to the hit point.
 *
 * @param ray The ray to check for hits.
 * @param tMin The minimum distance at which a hit can occur.
 * @param tMax The maximum distance at which a hit can occur.
 * @param payload Output parameter for information about the hit. Only modified if a hit occurs.
 * @return bool Returns true if the ray hits the sphere; otherwise, returns false.
 */
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

/**
 * @brief Calculates the closest hit point on the sphere and updates the hit payload.
 *
 * This function is called after a ray has been determined to hit the sphere. It calculates the
 * position of the closest hit point on the sphere and the normal at that point, and updates the hit
 * payload with this information.
 *
 * The position of the hit point is calculated by moving along the ray from its origin by the distance
 * to the hit point (which is stored in the hit payload).
 *
 * The normal at the hit point is calculated by subtracting the position of the sphere's center from
 * the position of the hit point and dividing by the sphere's radius. This gives a vector that points
 * outward from the sphere at the hit point.
 *
 * The function also sets the material index in the hit payload to the sphere's material index.
 *
 * @param ray The ray that hit the sphere.
 * @param payload The hit payload containing information about the hit. Modified by this function.
 */
void Sphere::ClosestHit(const Ray &ray, HitPayload &payload) const
{
    payload.position = ray.Origin + ray.Direction * payload.HitDistance;
    glm::vec3 outwardNormal = (payload.position - Position) / Radius;
    payload.setFaceNormal(ray, outwardNormal);

    payload.materialIndex = MaterialIndex;
}

/**
 * @brief Renders the GUI options for the sphere object.
 *
 * This function uses ImGui to render a GUI that allows the user to modify the properties of the sphere.
 * The user can change the position and radius of the sphere, and select a material from a list of
 * available materials.
 *
 * The function returns true if any of the options were changed by the user; otherwise, it returns false.
 *
 * @param materialNames A vector of names of available materials. The user can select a material for
 * the sphere from this list.
 * @return bool Returns true if any of the options were changed by the user; otherwise, returns false.
 */
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
