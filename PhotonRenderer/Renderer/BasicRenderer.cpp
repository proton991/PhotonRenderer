#include "BasicRenderer.hpp"

#include <memory>
#include "Graphics/Framebuffer.hpp"
#include "Graphics/Shader.hpp"
#include "Line.hpp"
#include "RenderAPI.hpp"

namespace photon {

BasicRenderer::BasicRenderer(const RendererConfig& config)
    : m_width(config.width), m_height(config.height) {
  ShaderProgramCreateInfo info1{"pbr",
                                {
                                    {"Data/Shaders/forward.vs.glsl", "vertex"},
                                    {"Data/Shaders/pbr_cook_torrance.fs.glsl", "fragment"},
                                }};
  ShaderProgramCreateInfo info2{"screen",
                                {
                                    {"Data/Shaders/framebuffers_screen.vs.glsl", "vertex"},
                                    {"Data/Shaders/framebuffers_screen.fs.glsl", "fragment"},
                                }};
  ShaderProgramCreateInfo info3{"lines",
                                {
                                    {"Data/Shaders/lines.vs.glsl", "vertex"},
                                    {"Data/Shaders/lines.fs.glsl", "fragment"},
                                }};
  m_shaderCache.try_emplace(info3.name, ShaderProgramFactory::CreateShaderProgram(info3));
  CompileShaders({info1, info2, info3});
  SetupUbos();
  SetupScreenQuad();
  SetupFramebuffers(m_width, m_height);
  SetupCoordinateAxis();
  m_aabbLine  = CreateRef<Line>();
  m_shadowMap = CreateRef<ShadowMap>(1024, 1024);
}

void BasicRenderer::CompileShaders(
    const std::vector<ShaderProgramCreateInfo>& shader_program_infos) {
  // build shaders
  for (const auto& info : shader_program_infos) {
    auto shader_program = ShaderProgramFactory::CreateShaderProgram(info);
    m_shaderCache.try_emplace(info.name, std::move(shader_program));
  }
}

void BasicRenderer::SetupUbos() {
  m_cameraUBO     = gl::UniformBuffer::Create(sizeof(CameraData), 0);
  m_modelUBO      = gl::UniformBuffer::Create(sizeof(ModelData), 1);
  m_pbrSamplerUBO = gl::UniformBuffer::Create(sizeof(ModelData), 2);
}

void BasicRenderer::SetupScreenQuad() {
  float quadVertices[] = {
      // positions   // texCoords
      -1.0f, 1.0f,  0.0f, 1.0f,  //
      -1.0f, -1.0f, 0.0f, 0.0f,  //
      1.0f,  -1.0f, 1.0f, 0.0f,  //

      -1.0f, 1.0f,  0.0f, 1.0f,  //
      1.0f,  -1.0f, 1.0f, 0.0f,  //
      1.0f,  1.0f,  1.0f, 1.0f   //
  };
  m_quadVAO = gl::VertexArray::Create();
  m_quadVAO->Bind();
  auto vbo = VertexBuffer::Create(sizeof(quadVertices), quadVertices);
  vbo->SetBufferView({
      {"aPos", BufferDataType::Vec2f},
      {"aTexCoords", BufferDataType::Vec2f},
  });
  m_quadVAO->AttachVertexBuffer(vbo);
  m_quadVAO->Unbind();
}

void BasicRenderer::SetupFramebuffers(uint32_t width, uint32_t height) {
  // srgb color attachment
  AttachmentInfo color_info =
      AttachmentInfo::Color("color", AttachmentBinding::COLOR0, width, height);
  color_info.internalFormat = GL_SRGB8_ALPHA8;
  AttachmentInfo depth{AttachmentInfo::Depth(width, height)};
  std::vector<AttachmentInfo> attachment_infos{color_info, depth};
  FramebufferCreatInfo framebuffer_ci{width, height, attachment_infos};
  m_pbuffer = gl::Framebuffer::Create(framebuffer_ci);
}

void BasicRenderer::SetupCoordinateAxis() {
  m_axisLine               = CreateRef<Line>();
  m_axisLine->lineVertices = {
      // x axis
      {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
      {glm::vec3(100.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
      // y axis
      {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
      {glm::vec3(0.0f, 100.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
      // z axis
      {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
      {glm::vec3(0.0f, 0.0f, 100.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
  };
  auto vbo = VertexBuffer::Create(m_axisLine->lineVertices.size() * sizeof(LineVertex),
                                  m_axisLine->lineVertices.data());
  vbo->SetBufferView({
      {"aPosition", BufferDataType::Vec3f},
      {"aColor", BufferDataType::Vec4f},
  });
  m_axisLine->vao = gl::VertexArray::Create();
  m_axisLine->vao->Bind();
  m_axisLine->vao->AttachVertexBuffer(vbo);
}

void BasicRenderer::RenderModel(const Ref<asset::Model>& model) {
  RenderMeshes(model->GetMeshes());
}

void BasicRenderer::SetDefaultState() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_FRAMEBUFFER_SRGB);
  RenderAPI::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
  RenderAPI::EnableBlending(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void BasicRenderer::UpdateUbo(const FrameInfo& info) {
  // camera ubo
  m_cameraData.viewMatrix     = info.camera->GetViewMatrix();
  m_cameraData.projMatrix     = info.camera->GetProjectionMatrix();
  m_cameraData.projViewMatrix = m_cameraData.projMatrix * m_cameraData.viewMatrix;
  m_cameraUBO->SetData(&m_cameraData, sizeof(CameraData));
  // scene ubo
  auto& shader = m_shaderCache.at("pbr");
  shader->Use();
  shader->SetUniform("uLightIntensity", info.scene->GetLightIntensity());
  shader->SetUniform("uLightPos", info.scene->GetLightPos());
  shader->SetUniform("uLightDir", info.scene->GetLightDir());
  shader->SetUniform("uLightType", static_cast<int>(info.options->lightType));
  shader->SetUniform("uCameraPos", info.camera->GetPos());
  shader->SetUniform("uLightSpaceMat", m_shadowMap->GetLightSpaceMatrix());
}

void BasicRenderer::RenderMeshes(const std::vector<Mesh>& meshes) {
  for (const auto& mesh : meshes) {
    m_samplerData = {};
    // model ubo
    m_modelData.modelMatrix = mesh.modelMatrix;
    m_modelUBO->SetData(&m_modelData, sizeof(ModelData));
    // bindless textures
    mesh.material.BindTextures(m_shaderCache.at("pbr"), m_samplerData);
    m_pbrSamplerUBO->SetData(m_samplerData.samplers, sizeof(PBRSamplerData));
    // draw mesh
    RenderAPI::DrawMesh(mesh);
  }
}

void BasicRenderer::RenderScene(const FrameInfo& info) {
  RenderAPI::EnableCullFace();
  info.scene->m_terrain->Draw(info.camera, info.scene->GetLightPos() - info.scene->GetCenter());
  RenderAPI::DisableCullFace();
  m_shadowMap->BindForRead(6);
  if (info.scene->HasSkybox()) {
    // bind Prefiltered IBL texture
    if (info.options->enableEnvMap) {
      info.scene->m_skybox->BindPrefilterData();
    } else {
      info.scene->m_skybox->UnbindPrefilterData();
    }
    if (info.options->showBackground) {
      info.scene->m_skybox->Draw(info.camera, info.options->blur);
    }
  }
  // render models
  for (const auto& model : info.scene->m_models) {
    RenderModel(model);
    if (info.options->showAABB) {
      model->GetAABB().FillLinesData(m_aabbLine);
      m_shaderCache.at("lines")->Use();
      RenderAPI::DrawLine(m_aabbLine->vao, m_aabbLine->lineVertices.size());
    }
  }
  if (info.options->showLightModel) {
    RenderModel(info.scene->m_lightModel);
  }
  if (info.options->showFloor) {
    RenderModel(info.scene->m_floor);
  }
  if (info.options->showAxis) {
    m_shaderCache.at("lines")->Use();
    RenderAPI::DrawLine(m_axisLine->vao, m_axisLine->lineVertices.size());
  }
}

void BasicRenderer::RenderFrame(const FrameInfo& info) {
  m_shadowMap->RunDepthPass(info.scene, info.options->lightType);
  SetDefaultState();
  m_pbuffer->BindForWriting();
  m_pbuffer->Clear();
  RenderAPI::EnableDepthTesting();

  UpdateUbo(info);
  RenderScene(info);

  m_pbuffer->Unbind();

  RenderAPI::DisableDepthTesting();
  RenderAPI::ClearColorBuffer();

  auto& screen_shader = m_shaderCache.at("screen");
  screen_shader->Use();
  if (info.options->showDepthDebug) {
    m_shadowMap->BindDebugTexture(info.options->lightType);
  } else {
    m_pbuffer->BindForReading("color", 0);
  }

  RenderAPI::DrawVertices(m_quadVAO, 6);
}

void BasicRenderer::ResizeFbos(int width, int height) {
  m_width  = width;
  m_height = height;
  SetupFramebuffers(m_width, m_height);
  glViewport(0, 0, width, height);
}
}  // namespace photon