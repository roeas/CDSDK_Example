#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

namespace
{
constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float SPEED = 0.2f;
constexpr float SENSITIVITY = 0.1f;
constexpr float ZOOM = 45.0f;
}

class Camera
{
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    glm::mat4 GetViewMatrix();

    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset);
    void ProcessMouseScroll(float yoffset);

    void FrameAll(const glm::vec3 &max, const glm::vec3 &min);

    void LookAt(const glm::vec3 &front);

    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    float m_yaw;
    float m_pitch;
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;

private:
    void UpdateCameraVectors();
};
