#include "ShadowMap.hpp"
#include "Common/Logging.hpp"
#include "Engine/Scene.hpp"
#include "Graphics/Framebuffer.hpp"
#include "RenderAPI.hpp"

namespace photon {
ShadowMap::ShadowMap(uint32_t width, uint32_t height) : m_width(width), m_height(height) {
  m_depthShader = ShaderProgramFactory::CreateShaderProgram(
      {"shadow_map_depth",
       {
           {"Data/Shaders/shadowmap_depth.vs.glsl", "vertex"},
           {"Data/Shaders/shadowmap_depth.fs.glsl", "fragment"},
       }});
  m_debugShader = ShaderProgramFactory::CreateShaderProgram(
      {"shadow_map_depth",
       {
           {"Data/Shaders/debug_quad_depth.vs.glsl", "vertex"},
           {"Data/Shaders/debug_quad_depth.fs.glsl", "fragment"},
       }});
  SetupFramebuffer();
}

void ShadowMap::RunDepthPass(const Ref<BaseScene>& scene, const LightType& type) {
  const auto& aabb = scene->GetAABB();
  auto aabb_len    = glm::length(aabb.diag);
  // set near far plane
  m_near             = 0.01f * aabb_len;
  m_far              = 10.0f * aabb_len;
  const auto box_len = aabb_len * 2;

  glm::mat4 lightViewMatrix{1.0f};
  glm::mat4 lightProjMatrix{1.0f};
  if (type == LightType::Directional) {
    lightProjMatrix = glm::ortho(-box_len, box_len, -box_len, box_len, m_near, m_far);
    // for directional light, fix the light position
    lightViewMatrix =
        glm::lookAt(scene->GetAABB().posMax * 5.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  } else {
    lightProjMatrix = glm::perspective(glm::radians(45.0f), 1.0f, m_near, m_far);
    lightViewMatrix =
        glm::lookAt(scene->GetLightPos(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  }
  m_lightSpaceMatrix = lightProjMatrix * lightViewMatrix;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  glViewport(0, 0, m_width, m_height);
  // Clear the depth buffer of the shadow map
  glClearNamedFramebufferfv(m_fbo, GL_DEPTH, 0, &ClearDepth);
  m_depthShader->Use();
  m_depthShader->SetUniform("uLightSpaceMat", m_lightSpaceMatrix);
  for (const auto& model : scene->m_models) {
    for (const auto& mesh : model->GetMeshes()) {
      m_depthShader->SetUniform("uModelMat", mesh.modelMatrix);
      RenderAPI::DrawMesh(mesh);
    }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::SetupFramebuffer() {
  glCreateFramebuffers(1, &m_fbo);
  glCreateTextures(GL_TEXTURE_2D, 1, &m_depthTexture);
  glTextureParameteri(m_depthTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(m_depthTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTextureParameteri(m_depthTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTextureParameteri(m_depthTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTextureStorage2D(m_depthTexture, 1, GL_DEPTH_COMPONENT32F, m_width, m_height);
  const float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTextureParameterfv(m_depthTexture, GL_TEXTURE_BORDER_COLOR, borderColor);
  glNamedFramebufferTexture(m_fbo, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);
  glNamedFramebufferDrawBuffer(m_fbo, GL_NONE);
  glNamedFramebufferReadBuffer(m_fbo, GL_NONE);
  auto status = glCheckNamedFramebufferStatus(m_fbo, GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    LOGI("Framebuffer is not complete, status {}", status);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::BindForRead(int slot) {
  glBindTextureUnit(slot, m_depthTexture);
  //  m_fbo->bind_for_reading("depth", slot);
}

void ShadowMap::BindDebugTexture(const LightType& type) {
  m_debugShader->Use();
  m_debugShader->SetUniform("uNear", m_near);
  m_debugShader->SetUniform("uFar", m_far);
  m_debugShader->SetUniform("uLightType", static_cast<int>(type));
  glBindTextureUnit(0, m_depthTexture);
}
}  // namespace photon