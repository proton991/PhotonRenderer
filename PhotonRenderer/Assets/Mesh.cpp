#include "Mesh.hpp"

namespace photon::asset {
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    : numVertices(vertices.size()), numIndices(indices.size()) {
  Setup(vertices, indices);
}

Mesh::Mesh(const std::vector<Vertex>& vertices) : numVertices(vertices.size()), numIndices(0) {
  Setup(vertices);
}

void Mesh::Setup(const std::vector<Vertex>& vertices) {
  //  vao = std::make_unique<SimpleVAO>();
  vao = gl::VertexArray::Create();
  vao->Bind();
  auto vbo = gl::VertexBuffer::Create(vertices.size() * sizeof(Vertex), vertices.data());
  vbo->SetBufferView({
      {"aPos", gl::BufferDataType::Vec3f},
      {"aTexCoords", gl::BufferDataType::Vec2f},
      {"aNormal", gl::BufferDataType::Vec3f},
  });
  vao->AttachVertexBuffer(vbo);
}

void Mesh::Setup(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
  vao = gl::VertexArray::Create();
  vao->Bind();
  auto vbo = gl::VertexBuffer::Create(vertices.size() * sizeof(Vertex), vertices.data());
  vbo->SetBufferView({
      {"aPos", gl::BufferDataType::Vec3f},
      {"aTexCoords", gl::BufferDataType::Vec2f},
      {"aNormal", gl::BufferDataType::Vec3f},
  });

  auto ibo = gl::IndexBuffer::Create(indices.size(), indices.data());
  vao->AttachVertexBuffer(vbo);
  vao->AttachIndexBuffer(ibo);
}

}  // namespace photon::asset