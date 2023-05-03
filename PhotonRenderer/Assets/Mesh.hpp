#pragma once

#include <glm/ext/matrix_float4x4.hpp>

#include <vector>
#include "Graphics/VertexArray.hpp"
#include "Material.hpp"
#include "Renderer/Vertex.hpp"

namespace photon::asset {

struct Mesh {
  Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
  explicit Mesh(const std::vector<Vertex>& vertices);

  const GLsizei numVertices;
  const GLsizei numIndices;

  Ref<gl::VertexArray> vao;
  glm::mat4 modelMatrix{1.0f};
  PBRMaterial material;

private:
  void Setup(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
  void Setup(const std::vector<Vertex>& vertices);
};

}  // namespace photon::asset
