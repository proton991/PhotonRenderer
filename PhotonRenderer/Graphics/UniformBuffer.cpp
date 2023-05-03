#include "UniformBuffer.hpp"
#include <glad/glad.h>
namespace photon::gl {

UniformBuffer::UniformBuffer(uint32_t size, uint32_t binding) {
  glCreateBuffers(1, &m_id);
  glNamedBufferData(m_id, size, nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_id);
}

UniformBuffer::~UniformBuffer() {
  glDeleteBuffers(1, &m_id);
}

void UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset) const {
  glNamedBufferSubData(m_id, offset, size, data);
}
}  // namespace photon::gl