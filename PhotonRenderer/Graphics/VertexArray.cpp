#include "VertexArray.hpp"
#include "Common/Logging.hpp"

namespace photon::gl {

VertexArray::VertexArray() {
  glCreateVertexArrays(1, &m_id);
}

VertexArray::~VertexArray() {
  glDeleteVertexArrays(1, &m_id);
}

void VertexArray::Bind() const {
  glBindVertexArray(m_id);
}

void VertexArray::Unbind() const {
  glBindVertexArray(0);
}

void VertexArray::AttachVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
  assert(!vertexBuffer->GetBufferView().get_elements().empty());
  glBindVertexArray(m_id);
  vertexBuffer->Bind();
  const auto& view     = vertexBuffer->GetBufferView();
  const auto& elements = view.get_elements();
  for (int i = 0; i < elements.size(); ++i) {
    glEnableVertexAttribArray(i);
    // only support floats/vecNf for now
    glVertexAttribPointer(i, elements[i].count, GL_FLOAT, GL_FALSE, view.get_stride(),
                          reinterpret_cast<void*>(elements[i].offset));
  }
  m_vertexBuffers.push_back(vertexBuffer);
}

void VertexArray::AttachIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
  glBindVertexArray(m_id);
  indexBuffer->Bind();
  m_indexBuffer = indexBuffer;
}
}  // namespace photon::gl