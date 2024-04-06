#pragma once

#include <glm/glm.hpp>
#include "imgui.h"
#include <vector>

class Camera
{
public:
    Camera(float verticalFOV, float nearClip, float farClip, glm::vec3 position);

    bool OnUpdate(float ts);
    void OnResize(uint32_t width, uint32_t height);

    const glm::mat4 &GetProjection() const { return m_Projection; }
    const glm::mat4 &GetInverseProjection() const { return m_InverseProjection; }
    const glm::mat4 &GetView() const { return m_View; }
    const glm::mat4 &GetInverseView() const { return m_InverseView; }

    const glm::vec3 &GetPosition() const { return m_Position; }
    const glm::vec3 &GetDirection() const { return m_ForwardDirection; }

    const std::vector<glm::vec3> &GetRayDirections() const { return m_RayDirections; }

    const uint32_t GetViewportWidth() const { return m_ViewportWidth; }
    const uint32_t GetViewportHeight() const { return m_ViewportHeight; }

    void GetRayDirectionNearCurrentPosition(uint32_t x, uint32_t y);
    const glm::vec3 &GetRandomRayDirection(uint32_t x, uint32_t y)
    {
        GetRayDirectionNearCurrentPosition(x, y);
        return m_RayRandomDirection;
    }

    float GetRotationSpeed();
    bool RenderCameraOptions();

    const float &getAperatureSize() const { return m_AperureSize; }
    const float &getFocusDistance() const { return m_FocusDistance; }

private:
    void RecalculateProjection();
    void RecalculateView();
    void RecalculateRayDirections();

private:
    glm::mat4 m_Projection{1.0f};
    glm::mat4 m_View{1.0f};
    glm::mat4 m_InverseProjection{1.0f};
    glm::mat4 m_InverseView{1.0f};

    float m_VerticalFOV = 45.0f;
    float m_NearClip = 0.1f;
    float m_FarClip = 100.0f;

    float m_AperureSize = 0.0f;
    float m_FocusDistance = 10.0f;

    glm::vec3 m_Position{0.0f, 0.0f, 0.0f};
    glm::vec3 m_ForwardDirection{0.0f, 0.0f, 0.0f};

    // Cached ray directions
    std::vector<glm::vec3> m_RayDirections;

    // Random directions for antialiasing
    glm::vec3 m_RayRandomDirection;

    glm::vec2 m_LastMousePosition{0.0f, 0.0f};

    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};
