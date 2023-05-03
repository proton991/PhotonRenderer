#pragma once

#include <glad/glad.h>
#include <vector>
#include "MeshBuffer.hpp"

namespace photon::gl {
enum BufferType : int { Array = GL_ARRAY_BUFFER, Element = GL_ELEMENT_ARRAY_BUFFER };
enum DrawMode : int { STATIC = GL_STATIC_DRAW, DYNAMIC = GL_DYNAMIC_DRAW, STREAM = GL_STREAM_DRAW };

class VertexArray {
public:
  static Ref<VertexArray> Create() { return CreateRef<VertexArray>(); }
  VertexArray();
  virtual ~VertexArray();

  void Bind() const;
  void Unbind() const;

  void AttachVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
  void AttachIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

  const Ref<IndexBuffer>& GetIndexBuffer() const { return m_indexBuffer; }

private:
  uint32_t m_id{0};
  std::vector<Ref<VertexBuffer>> m_vertexBuffers;
  Ref<IndexBuffer> m_indexBuffer;
};
}  // namespace photon::gl
