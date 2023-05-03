#include "Model.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "Common/Logging.hpp"

namespace photon::asset {

Model::Model(const std::string& name, const std::vector<Vertex>& vertices,
             const std::vector<GLuint>& indices)
    : m_name(name) {
  m_meshes.emplace_back(vertices, indices);
  LOGI("Model {} loaded: {} vertices, {} indices", name, vertices.size(), indices.size());
}

Model::Model(const std::string& name, const std::vector<Vertex>& vertices) : m_name(name) {
  m_meshes.emplace_back(vertices);
}

void Model::Translate(const glm::vec3& targetPos) {
  glm::vec3 delta = targetPos - m_aabb.GetCenter();
  for (auto& mesh : m_meshes) {
    auto translation = glm::translate(glm::mat4(1.0), delta);
    mesh.modelMatrix = translation * mesh.modelMatrix;
  }

  m_aabb.Translate(targetPos);
}

void Model::Rotate(float angle) {
  for (auto& mesh : m_meshes) {
    mesh.modelMatrix =
        glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)) * mesh.modelMatrix;
  }
}

void Model::Rotate(float angle, const glm::vec3& point) {
  auto t1 = glm::translate(glm::mat4(1), -point);
  auto r  = glm::rotate(glm::mat4(1), angle, glm::vec3(0.0f, 1.0f, 0.0f));
  auto t2 = glm::translate(glm::mat4(1), point);
  auto T  = t2 * r * t1;
  for (auto& mesh : m_meshes) {
    mesh.modelMatrix = T * mesh.modelMatrix;
  }
  m_aabb.posMin = T * glm::vec4(m_aabb.posMin, 0.0f);
  m_aabb.posMax = T * glm::vec4(m_aabb.posMax, 0.0f);
}

void Model::Scale(float factor) {
  for (auto& mesh : m_meshes) {
    const auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(factor));
    mesh.modelMatrix = scale * mesh.modelMatrix;
  }
  m_aabb.Scale(factor);
}
}  // namespace photon::asset