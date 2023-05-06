#pragma once

#include <vector>
#include "Common/Base.hpp"
#include "FrameInfo.hpp"
#include "RenderData.hpp"
#include "ShadowMap.hpp"

namespace photon {
// forward declaration
namespace gl {
class Framebuffer;
class VertexArray;
class UniformBuffer;
}  // namespace gl

struct Line;

struct RendererConfig {
  const uint32_t width;
  const uint32_t height;
};
class BasicRenderer {
public:
  explicit BasicRenderer(const RendererConfig& config);

  void RenderFrame(const FrameInfo& info);

  void ResizeFbos(int width, int height);

private:
  void CompileShaders(const std::vector<gl::ShaderProgramCreateInfo>& shader_program_infos);
  void SetupUbos();
  void SetupScreenQuad();
  void SetupFramebuffers(uint32_t width, uint32_t height);
  void SetupCoordinateAxis();

  void RenderModel(const Ref<asset::Model>& model);
  void RenderMeshes(const std::vector<asset::Mesh>& meshes);
  void RenderScene(const FrameInfo& info);

  void UpdateUbo(const FrameInfo& info);

  void SetDefaultState();

  uint32_t m_width{0};
  uint32_t m_height{0};

  ModelData m_modelData{};
  CameraData m_cameraData{};
  PBRSamplerData m_samplerData{};

  Ref<gl::UniformBuffer> m_modelUBO;
  Ref<gl::UniformBuffer> m_cameraUBO;
  Ref<gl::UniformBuffer> m_pbrSamplerUBO;

  Ref<gl::Framebuffer> m_pbuffer;

  Ref<gl::VertexArray> m_quadVAO;
  std::unordered_map<std::string, Ref<gl::ShaderProgram>> m_shaderCache;

  Ref<Line> m_axisLine;
  Ref<Line> m_aabbLine;

  Ref<ShadowMap> m_shadowMap;
};
}  // namespace photon
