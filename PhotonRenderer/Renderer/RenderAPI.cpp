#include "RenderAPI.hpp"
#include "Assets/Mesh.hpp"
#include "Assets/Model.hpp"
#include "Graphics/VertexArray.hpp"

namespace photon {

void RenderAPI::EnableBlending(int sfactor, int dfactor) {
  glEnable(GL_BLEND);
  glBlendFunc(sfactor, dfactor);
}

void RenderAPI::EnableCullFace() {
  // Enable face culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);  // Specify to cull the back faces
}

void RenderAPI::DisableCullFace() {
  glDisable(GL_CULL_FACE);
}

void RenderAPI::SetClearColor(const glm::vec4& color) {
  glClearColor(color.r, color.g, color.b, color.a);
}

void RenderAPI::ClearColorAndDepthBuffer() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderAPI::ClearColorBuffer() {
  glClear(GL_COLOR_BUFFER_BIT);
}

void RenderAPI::EnableDepthTesting() {
  glEnable(GL_DEPTH_TEST);
}

void RenderAPI::DisableDepthTesting() {
  glDisable(GL_DEPTH_TEST);
}

void RenderAPI::DrawLine(const std::shared_ptr<VertexArray>& vao, uint32_t num_vertices) {
  vao->Bind();
  glLineWidth(2.0f);
  glDrawArrays(GL_LINES, 0, num_vertices);
}

void RenderAPI::DrawVertices(const std::shared_ptr<VertexArray>& vao, uint32_t num_vertices) {
  vao->Bind();
  glDrawArrays(GL_TRIANGLES, 0, num_vertices);
}
void RenderAPI::DrawIndices(const std::shared_ptr<VertexArray>& vao) {
  vao->Bind();
  glDrawElements(GL_TRIANGLES, vao->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}

void RenderAPI::DrawMeshes(const std::vector<Mesh>& meshes) {
  for (const auto& mesh : meshes) {
    mesh.vao->Bind();
    glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, nullptr);
  }
}

void RenderAPI::DrawMesh(const Mesh& mesh) {
  mesh.vao->Bind();
  if (mesh.numIndices != 0) {
    glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, nullptr);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, mesh.numVertices);
  }
}
}  // namespace photon