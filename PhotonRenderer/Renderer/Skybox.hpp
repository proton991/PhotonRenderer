#pragma once

#include <string>
#include "Assets/Texture.hpp"
#include "Common/Base.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Systems/CameraSystem.hpp"

#define DIFFUSE_RESOLUTION 256
#define SPECULAR_RESOLUTION 512

namespace photon::util {
class AssetCache;
}
namespace photon {
namespace gl {
class Framebuffer;
}

enum class SkyboxType { Cubemap, Equirectangular };
class Skybox {
public:
  static Ref<Skybox> Create(const std::vector<std::string>& face_paths);
  static Ref<Skybox> Create(const std::string& hdr_path, int resolution);
  Skybox(const std::vector<std::string>& face_paths);   // using 6 images
  Skybox(const std::string& hdr_path, int resolution);  // using hdr image
  void Draw(const Ref<system::Camera>& camera, bool blur = false);
  // IBL
  void BindPrefilterData();
  void UnbindPrefilterData();

private:
  void SetupShaders();
  void SetupScreenQuads();
  void setup_cube_quads();

  void DrawCube();
  void DrawQuad();
  // IBL
  void CalcPrefilterDiffuse();
  void CalcPrefilterSpecular();
  void CalcBRDFIntegration();

  Ref<asset::TextureCubeMap> m_cubeTexture;
  Ref<gl::VertexArray> m_quadVAO;
  Ref<gl::VertexArray> m_cubeVAO;

  std::unordered_map<std::string, Ref<gl::ShaderProgram>> m_shaderCache;
  Ref<gl::Framebuffer> m_envFBO;
  Ref<gl::Framebuffer> m_screenFBO;
  SkyboxType m_type;

  int m_resolution{0};
  const unsigned int MaxMipLevels = 5;

  Unique<util::AssetCache> m_assetCache;
};
}  // namespace photon
