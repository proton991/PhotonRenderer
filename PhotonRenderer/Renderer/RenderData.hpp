#pragma once

#include <glm/mat4x4.hpp>

namespace photon {
struct ModelData {
  glm::mat4 modelMatrix;
};

struct CameraData {
  glm::mat4 projViewMatrix;
  glm::mat4 viewMatrix;
  glm::mat4 projMatrix;
};

struct PBRSamplerData {
  GLuint64 samplers[5];
};
}  // namespace photon
