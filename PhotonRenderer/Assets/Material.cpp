#include "Material.hpp"

namespace photon::asset {
using namespace photon::gl;
void PBRMaterial::BindTextures(Ref<ShaderProgram>& shader, PBRSamplerData& data) const {
  shader->Use();
  shader->SetUniform("uBaseColorFactor", baseColorFactor);
  shader->SetUniform("uMetallicFactor", (float)metallicFactor);
  shader->SetUniform("uRoughnessFactor", (float)roughnessFactor);
  shader->SetUniform("uEmissiveFactor", emissiveFactor);
  shader->SetUniform("uOcclusionStrength", (float)occlusionStrength);
  shader->SetUniform("uAlphaMode", alphaMode);
  shader->SetUniform("uAlphaCutoff", alphaCutoff);

  if (textures.contains(PBRComponent::BaseColor)) {
    data.samplers[0] = textures.at(PBRComponent::BaseColor)->GetHandle();
    shader->SetUniform("uHasBaseColorMap", true);
  } else {
    shader->SetUniform("uHasBaseColorMap", false);
  }
  if (textures.contains(PBRComponent::MetallicRoughness)) {
    data.samplers[1] = textures.at(PBRComponent::MetallicRoughness)->GetHandle();
    shader->SetUniform("uHasMetallicRoughnessMap", true);
  } else {
    shader->SetUniform("uHasMetallicRoughnessMap", false);
  }

  if (textures.contains(PBRComponent::Emissive)) {
    data.samplers[2] = textures.at(PBRComponent::Emissive)->GetHandle();
    shader->SetUniform("uHasEmissiveMap", true);
  } else {
    shader->SetUniform("uHasEmissiveMap", false);
  }

  if (textures.contains(PBRComponent::Occlusion)) {
    data.samplers[3] = textures.at(PBRComponent::Occlusion)->GetHandle();
    shader->SetUniform("uHasOcclusionMap", true);
  } else {
    shader->SetUniform("uHasOcclusionMap", false);
  }

  if (textures.contains(PBRComponent::Normal)) {
    data.samplers[4] = textures.at(PBRComponent::Normal)->GetHandle();
    shader->SetUniform("uHasNormalMap", true);
  } else {
    shader->SetUniform("uHasNormalMap", false);
  }
}
}  // namespace photon::asset