#pragma once

#include "Common/Base.hpp"

namespace photon::gl {

class UniformBuffer {
public:
  static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding) {
    return CreateRef<UniformBuffer>(size, binding);
  }
  UniformBuffer(uint32_t size, uint32_t binding);
  virtual ~UniformBuffer();

  void SetData(const void* data, uint32_t size, uint32_t offset = 0) const;

private:
  uint32_t m_id{0};
};
}  // namespace photon::gl
