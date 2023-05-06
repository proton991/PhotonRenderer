#include "CameraSystem.hpp"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "Platform/NativeInput.hpp"

namespace photon::system {
using namespace photon::platform;
Ref<Camera> Camera::Create(const glm::vec3& bbox_min, const glm::vec3& bbox_max, float aspect) {
  const auto diag   = bbox_max - bbox_min;
  auto max_distance = glm::length(diag);
  float near        = 0.001f * max_distance;
  float far         = 100.f * max_distance;
  float fov         = 70.0f;
  const auto center = 0.5f * (bbox_max + bbox_min);
  const auto up     = glm::vec3(0, 1, 0);
  //  const auto eye    = diag.z > 0 ? center + 1.5f * diag : center + 2.f * glm::cross(diag, up);
  // place camera at the bbx corner
  const auto eye   = glm::vec3(bbox_max.x, bbox_max.y, bbox_max.z);
  const auto speed = max_distance / 2;
  return CreateRef<Camera>(eye, center, fov, aspect, near, far, speed);
}

Camera Camera::CreateDefault() {
  glm::vec3 eye    = {0.0f, 0.0f, 5.0f};
  glm::vec3 target = {0.0f, 0.0f, 0.0f};
  float fov        = 70.0f;
  float near       = 1.0f;
  float far        = 100.0f;

  return {eye, target, fov, 1.0f, near, far};
}

Camera Camera::CreateBasedOnBBox(const glm::vec3& bbox_min, const glm::vec3& bbox_max) {
  const auto diag   = bbox_max - bbox_min;
  auto maxDistance  = glm::length(diag);
  float near        = 0.001f * maxDistance;
  float far         = 100.f * maxDistance;
  float fov         = 70.0f;
  const auto center = 0.5f * (bbox_max + bbox_min);
  const auto up     = glm::vec3(0, 1, 0);
  const auto eye    = diag.z > 0 ? center + 1.5f * diag : center + 2.f * glm::cross(diag, up);
  const auto speed  = maxDistance / 2;
  return {eye, center, fov, 1.0f, near, far, speed};
}
Camera::Camera(glm::vec3 eye, glm::vec3 target, float fov, float aspect, float near, float far,
               float speed)
    : m_position{eye}, m_aspect{aspect}, m_fov{fov}, m_near{near}, m_far{far}, m_speed(speed) {
  glm::vec3 direction = glm::normalize(target - m_position);

  m_pitch  = glm::degrees(asin(direction.y));
  m_yaw    = glm::degrees(atan2(direction.z, direction.x));
  m_target = target;

  UpdateBaseVectors();
  SetProjectionMatrix();
}

void Camera::UpdateBaseVectors() {
  m_front.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
  m_front.y = glm::sin(glm::radians(m_pitch));
  m_front.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
  // Calculate the new Front vector

  m_front = glm::normalize(m_front);
  m_right = glm::normalize(glm::cross(m_front, m_world_up));
  m_up    = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::SetProjectionMatrix() {
  assert(glm::abs(m_aspect - std::numeric_limits<float>::epsilon()) > 0.f);
  m_projMatrix = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
}

void Camera::UpdatePosition(float velocity) {
  if (KeyboardMouseInput::GetInstance().WasKeyPressedOnce(GLFW_KEY_UP)) {
    m_speed *= 2;
  }
  if (KeyboardMouseInput::GetInstance().WasKeyPressedOnce(GLFW_KEY_DOWN)) {
    m_speed /= 2;
  }
  if (KeyboardMouseInput::GetInstance().IsKeyPressed(GLFW_KEY_SPACE)) {
    // reset position
    m_position = {0.0f, 0.0f, 0.0f};
  }
  if (KeyboardMouseInput::GetInstance().IsKeyPressed(GLFW_KEY_W)) {
    m_position += m_front * velocity;
  }
  if (KeyboardMouseInput::GetInstance().IsKeyPressed(GLFW_KEY_S)) {
    m_position -= m_front * velocity;
  }
  if (KeyboardMouseInput::GetInstance().IsKeyPressed(GLFW_KEY_A)) {
    m_position -= m_right * velocity;
  }
  if (KeyboardMouseInput::GetInstance().IsKeyPressed(GLFW_KEY_D)) {
    m_position += m_right * velocity;
  }
  if (KeyboardMouseInput::GetInstance().IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
    m_position += m_world_up * velocity;
  }
  if (KeyboardMouseInput::GetInstance().IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
    m_position -= m_world_up * velocity;
  }
}

void Camera::UpdateView() {
  auto delta = KeyboardMouseInput::GetInstance().CalculateCursorPositionDelta();
  m_yaw += delta[0] * m_sensitivity;
  m_pitch += delta[1] * m_sensitivity;
  m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
}

void Camera::Update(float deltaTime, bool rotate) {
  if (KeyboardMouseInput::GetInstance().WasKeyPressedOnce(GLFW_KEY_TAB) && !rotate) {
    m_dirty = !m_dirty;
    if (m_dirty) {
      KeyboardMouseInput::GetInstance().Resume();
    } else {
      KeyboardMouseInput::GetInstance().Pause();
    }
  }
  if (m_dirty && !rotate) {
    const float velocity = m_speed * deltaTime;
    UpdatePosition(velocity);
    UpdateView();
    UpdateBaseVectors();
  }

  if (rotate && !m_dirty) {
    float angle = 0.3f * deltaTime;
    auto rotation{glm::mat3(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)))};
    m_position = m_position * rotation;
    m_front    = m_front * rotation;
    m_up       = m_up * rotation;
    m_right    = m_right * rotation;
    glm::vec3 direction{glm::normalize(m_target - m_position)};
    m_pitch = glm::degrees(asin(direction.y));
    m_yaw   = glm::degrees(atan2(direction.z, direction.x));
  }
}

void Camera::UpdateAspect(float aspect) {
  m_aspect = aspect;
  SetProjectionMatrix();
}

glm::mat4 Camera::GetViewMatrix() const {
  return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
  return m_projMatrix;
}
}  // namespace photon::system