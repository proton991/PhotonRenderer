#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace photon {
namespace asset {
class Model;
struct Mesh;
}  // namespace asset

namespace gl {
class VertexArray;
}
using namespace gl;
using namespace asset;
class RenderAPI {
public:
  static void SetClearColor(const glm::vec4& color);
  static void ClearColorAndDepthBuffer();
  static void ClearColorBuffer();
  static void EnableDepthTesting();
  static void DisableDepthTesting();
  static void EnableBlending(int sfactor, int dfactor);

  static void DrawLine(const std::shared_ptr<gl::VertexArray>& vao, uint32_t num_vertices);
  static void DrawVertices(const std::shared_ptr<VertexArray>& vao, uint32_t num_vertices);
  static void DrawIndices(const std::shared_ptr<VertexArray>& vao);

  static void DrawMeshes(const std::vector<Mesh>& meshes);
  static void DrawMesh(const Mesh& mesh);
};

}  // namespace photon
