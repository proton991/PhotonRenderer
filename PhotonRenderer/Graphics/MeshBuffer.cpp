#include "MeshBuffer.hpp"

#include <glad/glad.h>

namespace photon::gl {
void BufferView::calc_stride_and_offset() {
  size_t offset = 0;
  for (auto& ele : m_elements) {
    m_stride += ele.size;
    ele.offset = offset;
    offset += ele.size;
  }
}
VertexBuffer::VertexBuffer(uint32_t size) {
  glCreateBuffers(1, &m_id);
  glBindBuffer(GL_ARRAY_BUFFER, m_id);
  glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
}
VertexBuffer::VertexBuffer(uint32_t size, const void* data) {
  glCreateBuffers(1, &m_id);
  //  glBindBuffer(GL_ARRAY_BUFFER, m_id);
  glNamedBufferData(m_id, size, data, GL_STATIC_DRAW);
  //  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
  glDeleteBuffers(1, &m_id);
}

void VertexBuffer::Bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::Unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetData(uint32_t size, const void* data) {
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

IndexBuffer::IndexBuffer(uint32_t count, const void* data) : m_count(count) {
  glCreateBuffers(1, &m_id);

  glBindBuffer(GL_ARRAY_BUFFER, m_id);
  glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
  glDeleteBuffers(1, &m_id);
}

void IndexBuffer::Bind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::Unbind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
}  // namespace photon::gl