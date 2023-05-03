#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "Common/Base.hpp"
namespace photon::system {
class Camera {
public:
  static Ref<Camera> Create(const glm::vec3& bbox_min, const glm::vec3& bbox_max,
                            float aspect = 1.0f);
  static Camera CreateDefault();
  static Camera CreateBasedOnBBox(const glm::vec3& bbox_min, const glm::vec3& bbox_max);
  Camera(glm::vec3 eye, glm::vec3 target, float fov, float aspect, float near, float far,
         float speed = 2.0f);
  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetProjectionMatrix() const;
  const auto GetPos() const { return m_position; }

  void SetSpeed(float speed) { m_speed = speed; }
  void Update(float deltaTime, bool rotate = false);
  void UpdateAspect(float aspect);

private:
  void SetProjectionMatrix();
  void UpdateBaseVectors();
  void UpdatePosition(float velocity);
  void UpdateView();
  // camera attributes
  glm::vec3 m_position{0.0f, 0.0f, 0.0f};
  glm::vec3 m_front{0.0f, 0.0f, 1.0f};
  glm::vec3 m_up{0.0f, 1.0f, 0.0f};
  glm::vec3 m_right;
  const glm::vec3 m_world_up{0.0f, 1.0f, 0.0f};

  glm::vec3 m_target{};

  float m_aspect;
  float m_fov{70.0f};

  float m_yaw{-90.0f};
  float m_pitch{0.0f};

  float m_near{1.0f};
  float m_far{100.0f};

  glm::mat4 m_projMatrix{1.f};
  glm::mat4 m_viewMatrix{1.f};

  float m_speed{2.f};
  const float m_sensitivity{0.2f};

  // should we update camera
  bool m_dirty{false};
};

}  // namespace photon::system
