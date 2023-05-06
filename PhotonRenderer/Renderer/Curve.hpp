#pragma once
#include <glad/glad.h>
#include <vector>
#include "Common/Base.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"

namespace photon {
namespace system {
class Camera;
}
class Curve {
public:
  Curve();
  void Draw(const Ref<system::Camera>& camara);

private:
  uint32_t m_numPoints = 0;

  std::vector<glm::vec3> ControlPoints = {
      glm::vec3(-50.0f, 50.0f, -50.0f),  //
      glm::vec3(-25.0f, 50.0f, 50.0f),   //
      glm::vec3(25.0f, 50.0f, 50.0f),    //
      glm::vec3(50.0f, 50.0f, -50.0f)    //
  };
  GLuint vao, vbo;
  Ref<gl::ShaderProgram> m_shader;
  Ref<gl::VertexArray> m_vao;
};
}  // namespace photon