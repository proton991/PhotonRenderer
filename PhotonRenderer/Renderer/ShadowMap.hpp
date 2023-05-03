#pragma once

#include <glm/mat4x4.hpp>
#include "Common/Base.hpp"
#include "Engine/RenderOption.hpp"

namespace photon {
namespace gl {
class Framebuffer;
class ShaderProgram;
}  // namespace gl
class BaseScene;

class ShadowMap {
public:
  ShadowMap(uint32_t width, uint32_t height);
  void RunDepthPass(const Ref<BaseScene>& scene, const LightType& type);
  void BindForRead(int slot);
  void BindDebugTexture(const LightType& type);
  auto GetLightSpaceMatrix() const { return m_lightSpaceMatrix; }

private:
  void SetupFramebuffer();
  uint32_t m_fbo{0};
  uint32_t m_depthTexture{0};
  uint32_t m_width{0};
  uint32_t m_height{0};
  float m_near{0.1f};
  float m_far{10.f};
  glm::mat4 m_lightSpaceMatrix;
  Ref<gl::ShaderProgram> m_depthShader;
  Ref<gl::ShaderProgram> m_debugShader;
  const float ClearDepth = 1.0f;
};
}  // namespace photon
