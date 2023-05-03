#pragma once

#include <array>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <unordered_map>
#include "Graphics/Shader.hpp"
#include "Renderer/RenderData.hpp"
#include "Texture.hpp"

namespace photon::asset {

enum class PBRComponent : decltype(0) {
  BaseColor         = 0,
  MetallicRoughness = 1,
  Emissive          = 2,
  Occlusion         = 3,
  Normal            = 4,
};

struct MaterialFactors {
  glm::vec4 baseColorFactor{1, 1, 1, 1};
  float metallicFactor{1.0};
  float roughnessFactor{1.0};
  glm::vec3 emissiveFactor{0, 0, 0};
  float occlusionStrength{1.0};
};

struct PBRMaterial {
  void BindTextures(Ref<gl::ShaderProgram>& shader, PBRSamplerData& samplerData) const;

  std::unordered_map<PBRComponent, Ref<Texture2D>> textures;
  double metallicFactor{1.0};  // default 1
  double roughnessFactor{1.0};
  glm::vec4 baseColorFactor{1, 1, 1, 1};
  glm::vec3 emissiveFactor{0, 0, 0};
  double occlusionStrength{1.0};

  // 0: opaque  1: blend  2: mask
  int alphaMode{0};
  float alphaCutoff{0.5};
  std::string name;
};
}  // namespace photon::asset
