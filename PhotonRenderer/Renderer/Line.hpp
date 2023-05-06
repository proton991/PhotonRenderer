#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Graphics/VertexArray.hpp"
#include "Vertex.hpp"

namespace photon {

struct Line {
  Line() = default;
  std::vector<LineVertex> lineVertices;
  Ref<gl::VertexArray> vao;
};
}  // namespace photon
