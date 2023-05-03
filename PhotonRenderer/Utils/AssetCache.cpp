#include "AssetCache.hpp"
#include "Common/Logging.hpp"
#include "Renderer/Vertex.hpp"

namespace photon::util {
Ref<Model> AssetCache::RequestModel(const std::string& path) {
  if (m_modelCache.find(path) == m_modelCache.end()) {
    m_modelCache.try_emplace(path, m_loader->LoadModel(path));
  }
  return m_modelCache.at(path);
}

Ref<Texture2D> AssetCache::RequestTexture(const std::string& path) {
  if (m_textureCache.find(path) == m_textureCache.end()) {
    m_textureCache.try_emplace(path, m_loader->LoadTexture(path));
  }
  return m_textureCache.at(path);
}

AssetCache::AssetCache() {
  m_whiteTexture = Texture2D::CreateDefaultWhite();
  m_loader       = CreateUnique<AssetLoader>();
}

void AssetCache::RemoveModel(const std::string& path) {
  if (m_modelCache.contains(path)) {
    m_modelCache.erase(path);
  }
}

}  // namespace photon::util