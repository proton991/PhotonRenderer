#pragma once
#include <string>
#include <unordered_map>
#include "AssetLoader.hpp"
#include "Assets/Model.hpp"
#include "Assets/Texture.hpp"
using photon::asset::Model;
using photon::asset::Texture2D;
namespace photon::util {
class AssetCache {
public:
  AssetCache();
  Ref<Model> RequestModel(const std::string& path);
  void RemoveModel(const std::string& path);
  Ref<Texture2D> RequestTexture(const std::string& path);

private:
  std::unordered_map<std::string, Ref<Model>> m_modelCache;
  std::unordered_map<std::string, Ref<Texture2D>> m_textureCache;
  Ref<Texture2D> m_whiteTexture;
  Unique<AssetLoader> m_loader;
};
}  // namespace photon::util