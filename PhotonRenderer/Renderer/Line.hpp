#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Graphics/VertexArray.hpp"

namespace photon {
struct LineVertex {
  glm::vec3 position;
  glm::vec4 color;
};

struct Line {
  Line() = default;
  std::vector<LineVertex> lineVertices;
  Ref<gl::VertexArray> vao;
};
}  // namespace photon
