#include "Renderer.h"
#include "Utils.h"

void Renderer::OnResize(uint32_t width, uint32_t height)
{
    if (m_FinalImage)
    {
        // No resize necessary
        if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
            return;

        m_FinalImage->Resize(width, height);
        ResetFrameIndex();
    }
    else
    {
        m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
    }

    delete[] m_ImageData;
    m_ImageData = new uint32_t[width * height];

    delete[] m_AccumulationData;
    m_AccumulationData = new glm::vec4[width * height];

    m_ImageHorizontalIter.resize(width);
    m_ImageVerticalIter.resize(height);
}

void Renderer::Render(const Scene &scene, Camera &camera)
{
    m_ActiveScene = &scene;
    m_ActiveCamera = &camera;

    if (m_FrameIndex == 1)
        memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));

#pragma omp parallel for

    for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            glm::vec4 color = PerPixel(x, y);
            m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

            glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
            accumulatedColor /= (float)m_FrameIndex;

            accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
            m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
        }
    }

    m_FinalImage->SetData(m_ImageData);

    if (m_Settings.Accumulate)
        m_FrameIndex++;
    else
        m_FrameIndex = 1;
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
    int numSamples = m_Samples;
    glm::vec3 color(0.0f);

    for (int s = 0; s < numSamples; s++)
    {
        Ray ray;
        ray.Origin = m_ActiveCamera->GetPosition();
        if (m_Settings.EnableAntialiasing)
        {
            ray.Direction = m_ActiveCamera->GetRandomRayDirection(x, y);
        }
        else
        {
            ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];
        }
        glm::vec2 inUnitDisk = Utils::InUnitDisk();
        glm::vec3 offset{inUnitDisk.x * 0.5f * m_ActiveCamera->getAperatureSize(),
                         inUnitDisk.y * 0.5f * m_ActiveCamera->getAperatureSize(),
                         0.0f};
        ray.Origin += offset;
        ray.Direction = glm::normalize(m_ActiveCamera->getFocusDistance() * ray.Direction - offset);

        glm::vec3 contribution(1.0f);

        int bounces = m_Bounces;
        int i = 0;
        while (i < bounces)
        {
            glm::vec3 attenuation(1.0f);
            glm::vec3 scatteredDirection(0.0f);
            HitPayload payload = TraceRay(ray);
            if (payload.HitDistance < 0.0f)
            {
                color += m_ActiveScene->SkyColor * contribution;
                i = bounces; // Exit the loop
            }
            else
            {
                const int materialIndex = payload.materialIndex;
                const std::shared_ptr<Material> &material = m_ActiveScene->Materials.at(materialIndex);

                if (material->scatter(ray, payload, attenuation, scatteredDirection))
                {
                    // ray.Origin = payload.position + payload.normal * 0.0001f;
                    // ray.Origin = payload.position + ray.Direction * (-0.0001f);
                    ray.Origin = payload.position + scatteredDirection * 0.0001f;
                    ray.Direction = scatteredDirection;

                    contribution *= attenuation;
                    i++;
                }
                else
                {
                    contribution = glm::vec3(0.0f);
                    i = bounces; // Exit the loop
                }
            }
        }
    }
    if (numSamples > 1)
    {
        color /= static_cast<float>(numSamples); // Average the accumulated color samples
    }

    return glm::vec4(color, 1.0f);
}

HitPayload Renderer::TraceRay(const Ray &ray)
{

    float hitDistance = std::numeric_limits<float>::max();
    HitPayload payload;
    bool hitAnything = m_ActiveScene->Hittables.hit(ray, 0.001f, hitDistance, payload);

    if (!hitAnything)
        return Miss(ray);

    return ClosestHit(ray, payload);
}
HitPayload Renderer::ClosestHit(const Ray &ray, HitPayload &payload)
{

    const std::shared_ptr<Hittable> &closestObject = m_ActiveScene->Hittables.objects.at(payload.objectIndex);

    closestObject->ClosestHit(ray, payload);

    return payload;
}

HitPayload Renderer::Miss(const Ray &ray)
{
    HitPayload payload;
    payload.HitDistance = -1.0f;
    return payload;
}
