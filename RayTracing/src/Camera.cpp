#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Walnut/Input/Input.h"
#include "Utils.h"

using namespace Walnut;

Camera::Camera(float verticalFOV, float nearClip, float farClip, glm::vec3 position)
    : m_VerticalFOV(verticalFOV), m_NearClip(nearClip), m_FarClip(farClip), m_Position(position)
{
    m_ForwardDirection = glm::vec3(0, 0, -1);
    //m_Position = glm::vec3(0, 0, 6);
}

/**
 * @brief Updates the camera's position and orientation based on user input.
 *
 * This function is called every frame and updates the camera's position and orientation based on user
 * input. The user can move the camera using the W, A, S, D, Q, and E keys, and rotate the camera by
 * moving the mouse while holding down the right mouse button.
 *
 * The function first checks if the right mouse button is down. If it is not, the function sets the
 * cursor mode to normal and returns false.
 *
 * If the right mouse button is down, the function sets the cursor mode to locked and calculates the
 * change in mouse position since the last frame. It then updates the camera's position based on the
 * state of the W, A, S, D, Q, and E keys, and updates the camera's orientation based on the change in
 * mouse position.
 *
 * The function recalculates the camera's view matrix and ray directions if the camera's position or
 * orientation was changed.
 *
 * @param ts The time step, i.e., the time since the last frame.
 * @return bool Returns true if the camera's position or orientation was changed; otherwise, returns false.
 */
bool Camera::OnUpdate(float ts)
{
    glm::vec2 mousePos = Input::GetMousePosition();
    glm::vec2 delta = (mousePos - m_LastMousePosition) * 0.002f;
    m_LastMousePosition = mousePos;

    if (!Input::IsMouseButtonDown(MouseButton::Right))
    {
        Input::SetCursorMode(CursorMode::Normal);
        return false;
    }

    Input::SetCursorMode(CursorMode::Locked);

    bool moved = false;

    constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
    glm::vec3 rightDirection = glm::cross(m_ForwardDirection, upDirection);

    float speed = 5.0f;

    // Movement
    if (Input::IsKeyDown(KeyCode::W))
    {
        m_Position += m_ForwardDirection * speed * ts;
        moved = true;
    }
    else if (Input::IsKeyDown(KeyCode::S))
    {
        m_Position -= m_ForwardDirection * speed * ts;
        moved = true;
    }
    if (Input::IsKeyDown(KeyCode::A))
    {
        m_Position -= rightDirection * speed * ts;
        moved = true;
    }
    else if (Input::IsKeyDown(KeyCode::D))
    {
        m_Position += rightDirection * speed * ts;
        moved = true;
    }
    if (Input::IsKeyDown(KeyCode::Q))
    {
        m_Position -= upDirection * speed * ts;
        moved = true;
    }
    else if (Input::IsKeyDown(KeyCode::E))
    {
        m_Position += upDirection * speed * ts;
        moved = true;
    }

    // Rotation
    if (delta.x != 0.0f || delta.y != 0.0f)
    {
        float pitchDelta = delta.y * GetRotationSpeed();
        float yawDelta = delta.x * GetRotationSpeed();

        glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
                                                glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
        m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

        moved = true;
    }

    if (moved)
    {
        RecalculateView();
        RecalculateRayDirections();
    }

    return moved;
}


void Camera::OnResize(uint32_t width, uint32_t height)
{
    if (width == m_ViewportWidth && height == m_ViewportHeight)
        return;

    m_ViewportWidth = width;
    m_ViewportHeight = height;

    RecalculateProjection();
    RecalculateRayDirections();
}

float Camera::GetRotationSpeed()
{
    return 0.3f;
}

/**
 * @brief Recalculates the camera's projection matrix and its inverse.
 *
 * This function is called whenever the camera's field of view, viewport size, or near and far clip
 * distances change. It recalculates the camera's projection matrix and its inverse based on these
 * properties.
 *
 * The projection matrix is calculated using glm's `perspectiveFov` function, which creates a
 * perspective projection matrix based on the vertical field of view, viewport width and height, and
 * near and far clip distances.
 *
 * The inverse projection matrix is calculated by taking the inverse of the projection matrix. This
 * matrix is used to convert coordinates from clip space to world space.
 */
void Camera::RecalculateProjection()
{
    m_Projection = glm::perspectiveFov(glm::radians(m_VerticalFOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearClip, m_FarClip);
    m_InverseProjection = glm::inverse(m_Projection);
}

/**
 * @brief Recalculates the camera's view matrix and its inverse.
 *
 * This function is called whenever the camera's position or forward direction changes. It recalculates
 * the camera's view matrix and its inverse based on these properties.
 *
 * The view matrix is calculated using glm's `lookAt` function, which creates a view matrix that
 * transforms world coordinates to camera coordinates. The function takes the camera's position, the
 * target position (which is the camera's position plus its forward direction), and the up direction
 * (which is the y-axis in this case).
 *
 * The inverse view matrix is calculated by taking the inverse of the view matrix. This matrix is used
 * to convert coordinates from camera space to world space.
 */
void Camera::RecalculateView()
{
    m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, glm::vec3(0, 1, 0));
    m_InverseView = glm::inverse(m_View);
}

/**
 * @brief Recalculates the directions of the rays that are cast from the camera.
 *
 * This function is called whenever the camera's view or projection matrices change. It recalculates
 * the directions of the rays that are cast from the camera based on these matrices.
 *
 * The function first resizes the `m_RayDirections` vector to match the number of pixels in the
 * viewport. It then loops over each pixel in the viewport and calculates the direction of the ray
 * that is cast from that pixel.
 *
 * The direction of the ray is calculated by transforming the pixel's normalized coordinates (which
 * range from -1 to 1) to world space using the inverse view and projection matrices. The resulting
 * vector is the direction of the ray in world space.
 *
 * The function stores the ray directions in the `m_RayDirections` vector, which is used by the
 * `CastRay` function to cast rays from the camera.
 */
void Camera::RecalculateRayDirections()
{
    m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

    for (uint32_t y = 0; y < m_ViewportHeight; y++)
    {
        for (uint32_t x = 0; x < m_ViewportWidth; x++)
        {
            glm::vec2 coord = {(float)x / (float)m_ViewportWidth, (float)y / (float)m_ViewportHeight};
            coord = coord * 2.0f - 1.0f; // -1 -> 1

            glm::vec4 target = m_InverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
            glm::vec3 rayDirection = glm::vec3(m_InverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space

            m_RayDirections[x + y * m_ViewportWidth] = rayDirection;
        }
    }
}

void Camera::GetRayDirectionNearCurrentPosition(uint32_t x, uint32_t y)
{
    glm::vec2 coord = {x / (float)m_ViewportWidth, y / (float)m_ViewportHeight};
    float pixelSize = 1.0f / glm::min(m_ViewportWidth, m_ViewportHeight);
    float offsetX = (Utils::UInt() % 100) / 100.0f * pixelSize;
    float offsetY = (Utils::UInt() % 100) / 100.0f * pixelSize;

    coord.x += offsetX;
    coord.y += offsetY;

    coord = coord * 2.0f - 1.0f; // -1 -> 1

    glm::vec4 target = m_InverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
    glm::vec3 rayDirection = glm::vec3(m_InverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space

    m_RayRandomDirection = rayDirection;
}

/**
 * @brief Calculates the direction of a ray that is cast from a pixel near the current position.
 *
 * This function calculates the direction of a ray that is cast from a pixel near the current position
 * of the camera. The pixel is specified by its x and y coordinates in the viewport.
 *
 * The function first normalizes the pixel coordinates to the range 0 to 1 by dividing by the viewport
 * width and height. It then adds a random offset to the coordinates to simulate the effect of
 * antialiasing.
 *
 * The function then transforms the pixel coordinates to the range -1 to 1, and uses the inverse
 * projection and view matrices to transform the coordinates to world space. The resulting vector is
 * the direction of the ray in world space.
 *
 * The function stores the ray direction in the `m_RayRandomDirection` member variable, which can be
 * used by other functions to cast the ray.
 *
 * @param x The x-coordinate of the pixel in the viewport.
 * @param y The y-coordinate of the pixel in the viewport.
 */
bool Camera::RenderCameraOptions()
{
    bool changed = false;
    changed |= ImGui::DragFloat("Focus Distance", &m_FocusDistance, 0.1f, 0.0f,m_FarClip);
    changed |= ImGui::DragFloat("Lens Radius", &m_AperureSize, 0.1f, 0.0f, 1000.0f);

    return changed;
}
