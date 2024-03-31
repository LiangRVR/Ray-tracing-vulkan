#include "Renderer.h"

#include "Walnut/Random.h"

#include <bits/stdc++.h>

#define MT_OpenMP 1
#define MT_TBB 0

namespace Utils
{

    static uint32_t ConvertToRGBA(const glm::vec4 &color)
    {
        uint8_t r = (uint8_t)(color.r * 255.0f);
        uint8_t g = (uint8_t)(color.g * 255.0f);
        uint8_t b = (uint8_t)(color.b * 255.0f);
        uint8_t a = (uint8_t)(color.a * 255.0f);

        uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
        return result;
    }

    static uint32_t PCG_Hash(uint32_t input)
    {
        uint32_t state = input * 747796405u + 2891336453u;
        uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
    }

    static float RandomFloat(uint32_t &seed)
    {
        seed = PCG_Hash(seed);
        return (float)seed / (float)std::numeric_limits<uint32_t>::max();
    }
    static glm::vec3 InUnitSphere(uint32_t &seed)
    {
        return glm::normalize(glm::vec3(
            RandomFloat(seed) * 2.0f - 1.0f,
            RandomFloat(seed) * 2.0f - 1.0f,
            RandomFloat(seed) * 2.0f - 1.0f));
    }
}

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

#if MT_TBB
    uint32_t greatest = std::max(width, height);
    for (uint32_t i = 0; i < greatest; i++)
    {
        if (i < width)
            m_ImageHorizontalIter[i] = i;
        if (i < height)
            m_ImageVerticalIter[i] = i;
    }
#endif
}

void Renderer::Render(const Scene &scene, const Camera &camera)
{
    m_ActiveScene = &scene;
    m_ActiveCamera = &camera;

    if (m_FrameIndex == 1)
        memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));

#if MT_TBB

    std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
                  [this](uint32_t y)
                  {
                      std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
                                    [this, y](uint32_t x)
                                    {
                                        glm::vec4 color = PerPixel(x, y);
                                        m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

                                        glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
                                        accumulatedColor /= (float)m_FrameIndex;

                                        accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
                                        m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
                                    });
                  });
#elif MT_OpenMP
#pragma omp parallel for
#endif

#if !MT_TBB
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
#endif

    m_FinalImage->SetData(m_ImageData);

    if (m_Settings.Accumulate)
        m_FrameIndex++;
    else
        m_FrameIndex = 1;
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
    Ray ray;
    ray.Origin = m_ActiveCamera->GetPosition();
    ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

    glm::vec3 light(0.0f);
    glm::vec3 contribution(1.0f);

    int bounces = m_Bounces;
    for (int i = 0; i < bounces; i++)
    {
        HitPayload payload = TraceRay(ray);
        if (payload.HitDistance < 0.0f)
        {
            glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
            light += skyColor * contribution;
            break;
        }

        const int materialIndex = payload.materialIndex;
        const Material &material = m_ActiveScene->Materials[materialIndex];

        contribution *= material.Albedo;
        light += material.GetEmission();

        ray.Origin = payload.position + payload.normal * 0.0001f;
        // glm::vec3 random_vector = payload.WorldNormal + material.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f);
        /*  glm::vec3 random_vector = payload.WorldNormal + material.Roughness * Walnut::Random::InUnitSphere();
         ray.Direction = glm::reflect(ray.Direction,
                                      random_vector); */
        ray.Direction = glm::normalize(payload.normal + material.Roughness * Walnut::Random::InUnitSphere());
    }

    return glm::vec4(light, 1.0f);
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
