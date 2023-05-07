#pragma once

#include <string>
#include "Mesh.hpp"
#include "Renderer/AABB.hpp"

namespace photon::asset {
class Model {
public:
  static Ref<Model> Create(const std::string& name) { return CreateRef<Model>(name); }
  explicit Model(const std::string& name) : m_name(name) {}
  Model(const std::string& name, const std::vector<Vertex>& vertices,
        const std::vector<GLuint>& indices);

  Model(const std::string& name, const std::vector<Vertex>& vertices);
  [[nodiscard]] const std::vector<Mesh>& GetMeshes() const { return m_meshes; };

  void AttachMesh(const Mesh& mesh) { m_meshes.push_back(mesh); }

  void SetAABB(const AABB& aabb) { m_aabb = aabb; }

  [[nodiscard]] const AABB& GetAABB() const { return m_aabb; }
  [[nodiscard]] AABB& GetAABB() { return m_aabb; }

  void Translate(const glm::vec3& targetPos);
  void Rotate(float angle);
  void Rotate(float angle, const glm::vec3& point);
  void Scale(float factor);

  auto GetMeshSize() const { return m_meshes.size(); }
  auto GetName() const { return m_name; }

  PBRMaterial& GetSingleMeshMaterial() { return m_meshes[0].material; }

private:
  std::string m_name;
  std::vector<Mesh> m_meshes;
  AABB m_aabb{};
};

}  // namespace photon::asset
