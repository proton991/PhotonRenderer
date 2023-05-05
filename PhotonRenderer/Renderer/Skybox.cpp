#include "Skybox.hpp"
#include "Graphics/framebuffer.hpp"
#include "Renderer/RenderAPI.hpp"
#include "Utils/AssetCache.hpp"

namespace photon {
const float CUBE_VERTICES[] = {
    // positions
    -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,  //
    1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,  //

    -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,  //
    -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,   //

    1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,   //
    1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,  //

    -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  //
    1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  //

    -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,   //
    1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,  //

    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,  //
    1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f    //
};

const float QUAD_VERTICES[] = {
    // positions        // texture Coords
    -1.0f, 1.0f,  0.0f, 1.0f,  //
    -1.0f, -1.0f, 0.0f, 0.0f,  //
    1.0f,  1.0f,  1.0f, 1.0f,  //
    1.0f,  -1.0f, 1.0f, 0.0f,  //
};
// Setup projection and view matrices for capturing data onto the 6 cubemap face directions
const auto CaptureProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);

const glm::mat4 CaptureViews[]{
    glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),   //
    glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),  //
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),    //
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),  //
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),   //
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))   //
};

void Skybox::SetupShaders() {
  std::vector<ShaderProgramCreateInfo> shaderProgramInfos;
  if (m_type == SkyboxType::Cubemap) {
    ShaderProgramCreateInfo info{"skybox",
                                 {
                                     {"Data/Shaders/background.vs.glsl", "vertex"},
                                     {"Data/Shaders/background.fs.glsl", "fragment"},
                                 }};
    shaderProgramInfos.push_back(info);
  } else {
    ShaderProgramCreateInfo converterInfo{
        "equirectangular_converter",
        {
            {"Data/Shaders/skybox.vs.glsl", "vertex"},
            {"Data/Shaders/equirectangular_converter.fs.glsl", "fragment"},
        }};
    ShaderProgramCreateInfo skyboxInfo{"skybox",
                                       {
                                           {"Data/Shaders/background.vs.glsl", "vertex"},
                                           {"Data/Shaders/background_hdr.fs.glsl", "fragment"},
                                       }};
    ShaderProgramCreateInfo prefilterDiffuseInfo{
        "prefilter_diffuse",
        {
            {"Data/Shaders/skybox.vs.glsl", "vertex"},
            {"Data/Shaders/prefilter_diffuse.fs.glsl", "fragment"},
        }};
    ShaderProgramCreateInfo prefilterSpecularInfo{
        "prefilter_specular",
        {
            {"Data/Shaders/skybox.vs.glsl", "vertex"},
            {"Data/Shaders/prefilter_specular.fs.glsl", "fragment"},
        }};
    ShaderProgramCreateInfo brdfLutInfo{"brdf_integration",
                                        {
                                            {"Data/Shaders/framebuffers_screen.vs.glsl", "vertex"},
                                            {"Data/Shaders/brdf_integration.fs.glsl", "fragment"},
                                        }};
    shaderProgramInfos.push_back(converterInfo);
    shaderProgramInfos.push_back(skyboxInfo);
    shaderProgramInfos.push_back(prefilterDiffuseInfo);
    shaderProgramInfos.push_back(prefilterSpecularInfo);
    shaderProgramInfos.push_back(brdfLutInfo);
  }
  for (const auto& info : shaderProgramInfos) {
    auto shader_program = ShaderProgramFactory::CreateShaderProgram(info);
    m_shaderCache.try_emplace(info.name, std::move(shader_program));
  }
}

Ref<Skybox> Skybox::Create(const std::vector<std::string>& face_paths) {
  return CreateRef<Skybox>(face_paths);
}

Ref<Skybox> Skybox::Create(const std::string& hdr_path, int resolution) {
  return CreateRef<Skybox>(hdr_path, resolution);
}

void Skybox::SetupScreenQuads() {
  m_quadVAO = VertexArray::Create();
  m_quadVAO->Bind();
  auto vbo = VertexBuffer::Create(sizeof(QUAD_VERTICES), QUAD_VERTICES);
  vbo->SetBufferView({
      {"aPos", BufferDataType::Vec2f},
      {"aTexCoords", BufferDataType::Vec2f},
  });
  m_quadVAO->AttachVertexBuffer(vbo);
  m_quadVAO->Unbind();
}

void Skybox::setup_cube_quads() {
  m_cubeVAO = VertexArray::Create();
  m_cubeVAO->Bind();
  auto vbo = VertexBuffer::Create(sizeof(CUBE_VERTICES), CUBE_VERTICES);
  vbo->SetBufferView({
      {"aPos", BufferDataType::Vec3f},
  });
  m_cubeVAO->AttachVertexBuffer(vbo);
}

Skybox::Skybox(const std::vector<std::string>& face_paths) {
  m_type = SkyboxType::Cubemap;
  SetupShaders();
  setup_cube_quads();
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  //m_cube_texture = m_assetCache->RequestTexture("skybox1", face_paths);
}

Skybox::Skybox(const std::string& hdrPath, int resolution) : m_resolution(resolution) {
  // init asset cache
  m_assetCache = CreateUnique<util::AssetCache>();

  m_type = SkyboxType::Equirectangular;
  SetupShaders();
  setup_cube_quads();
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  AttachmentInfo baseColor{.width   = resolution,
                           .height  = resolution,
                           .type    = AttachmentType::TEXTURE_CUBEMAP,
                           .binding = AttachmentBinding::COLOR0,
                           .name    = "base_color",
                           // use float for hdr
                           .internalFormat = GL_RGB16F};

  AttachmentInfo depth{AttachmentInfo::Depth(resolution, resolution)};
  depth.internalFormat = GL_RGB16F;

  FramebufferCreatInfo envFboCI{.width            = static_cast<uint32_t>(resolution),
                                .height           = static_cast<uint32_t>(resolution),
                                .attachmentsInfos = {baseColor}};
  m_envFBO = gl::Framebuffer::Create(envFboCI);

  auto hdrTexture     = m_assetCache->RequestTexture(hdrPath);
  auto& convertShader = m_shaderCache.at("equirectangular_converter");

  convertShader->Use();
  hdrTexture->Bind(0);
  m_envFBO->BindForWriting();
  for (int i = 0; i < 6; i++) {
    convertShader->SetUniform("uProjView", CaptureProj * CaptureViews[i]);
    m_envFBO->AttachLayerTexture(i, "base_color");
    DrawCube();
  }
  m_envFBO->Unbind();
  // calculate prefiltered IBL data
  CalcPrefilterDiffuse();
  CalcPrefilterSpecular();
  CalcBRDFIntegration();
}

void Skybox::CalcPrefilterDiffuse() {
  glGenerateTextureMipmap(m_envFBO->GetTextureId("base_color"));
  AttachmentInfo prefilter_diffuse{.width   = DIFFUSE_RESOLUTION,
                                   .height  = DIFFUSE_RESOLUTION,
                                   .type    = AttachmentType::TEXTURE_CUBEMAP,
                                   .binding = AttachmentBinding::COLOR0,
                                   .name    = "prefilter_diffuse",
                                   // use float for hdr
                                   .internalFormat = GL_RGB16F};

  auto& prefilterDiffuseShader = m_shaderCache.at("prefilter_diffuse");
  prefilterDiffuseShader->Use();
  m_envFBO->BindForWriting(false);
  m_envFBO->AddAttachment(prefilter_diffuse);
  m_envFBO->ResizeDepthRenderbuffer(DIFFUSE_RESOLUTION, DIFFUSE_RESOLUTION);
  m_envFBO->BindForReading("base_color", 0);
  glViewport(0, 0, DIFFUSE_RESOLUTION, DIFFUSE_RESOLUTION);
  for (int i = 0; i < 6; i++) {
    prefilterDiffuseShader->SetUniform("uProjView", CaptureProj * CaptureViews[i]);
    m_envFBO->AttachLayerTexture(i, "prefilter_diffuse");
    DrawCube();
  }
  m_envFBO->Unbind();
}

void Skybox::CalcPrefilterSpecular() {
  AttachmentInfo prefilterSpecular{.width     = SPECULAR_RESOLUTION,
                                   .height    = SPECULAR_RESOLUTION,
                                   .level     = static_cast<int>(MaxMipLevels),
                                   .type      = AttachmentType::TEXTURE_CUBEMAP,
                                   .binding   = AttachmentBinding::COLOR0,
                                   .name      = "prefilter_specular",
                                   .minFilter = GL_LINEAR_MIPMAP_LINEAR,
                                   // use float for hdr
                                   .internalFormat = GL_RGB16F,
                                   .generateMipmap = true};

  auto& shader = m_shaderCache.at("prefilter_specular");
  shader->Use();
  m_envFBO->BindForWriting(false);
  m_envFBO->AddAttachment(prefilterSpecular);
  m_envFBO->BindForReading("base_color", 0);
  for (int mip = 0; mip < MaxMipLevels; ++mip) {
    // reisze framebuffer according to mip-level size.
    auto mipWidth  = static_cast<int>((SPECULAR_RESOLUTION)*std::pow(0.5, mip));
    auto mipHeight = static_cast<int>((SPECULAR_RESOLUTION)*std::pow(0.5, mip));
    m_envFBO->ResizeDepthRenderbuffer(mipWidth, mipHeight);
    glViewport(0, 0, mipWidth, mipHeight);

    float roughness = (float)mip / (float)(MaxMipLevels - 1);
    shader->SetUniform("uRoughness", roughness);
    for (auto i = 0; i < 6; ++i) {
      shader->SetUniform("uProjView", CaptureProj * CaptureViews[i]);
      m_envFBO->AttachLayerTexture(i, "prefilter_specular", mip);
      DrawCube();
    }
  }
  m_envFBO->Unbind();
}

void Skybox::CalcBRDFIntegration() {
  SetupScreenQuads();
  AttachmentInfo info{.width   = m_resolution,
                      .height  = m_resolution,
                      .type    = AttachmentType::TEXTURE_2D,
                      .binding = AttachmentBinding::COLOR0,
                      .name    = "brdf_integration",
                      // use float for hdr
                      .internalFormat = GL_RG16F};

  FramebufferCreatInfo screenFboCI{.width            = static_cast<uint32_t>(m_resolution),
                                   .height           = static_cast<uint32_t>(m_resolution),
                                   .attachmentsInfos = {info}};
  m_screenFBO  = gl::Framebuffer::Create(screenFboCI);
  auto& shader = m_shaderCache.at("brdf_integration");

  m_screenFBO->BindForWriting();
  m_screenFBO->Clear();
  shader->Use();
  DrawQuad();
  m_screenFBO->Unbind();
}

void Skybox::BindPrefilterData() {
  m_envFBO->BindForReading("prefilter_diffuse", 3);
  m_envFBO->BindForReading("prefilter_specular", 4);
  m_screenFBO->BindForReading("brdf_integration", 5);
}

void Skybox::UnbindPrefilterData() {
  glBindTextureUnit(3, 0);
  glBindTextureUnit(4, 0);
  glBindTextureUnit(5, 0);
}

void Skybox::Draw(const Ref<system::Camera>& camera, bool blur) {
  auto& skyboxShader = m_shaderCache.at("skybox");
  skyboxShader->Use();
  if (m_type == SkyboxType::Cubemap) {
    m_cubeTexture->Bind(0);
  } else {
    blur ? m_envFBO->BindForReading("prefilter_diffuse", 0)
         : m_envFBO->BindForReading("base_color", 0);
  }
  auto view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
  skyboxShader->SetUniform("uProjView", camera->GetProjectionMatrix() * view);
  DrawCube();
}

void Skybox::DrawQuad() {
  m_quadVAO->Bind();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  m_quadVAO->Unbind();
}

void Skybox::DrawCube() {
  RenderAPI::DrawVertices(m_cubeVAO, 36);
}
}  // namespace photon