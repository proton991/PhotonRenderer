#pragma once
#include <string>
#include <vector>
#include "Common/Base.hpp"
namespace photon::asset {
class Model;
class Texture2D;
}  // namespace photon::asset
using photon::asset::Model;
using photon::asset::Texture2D;
namespace photon::util {

class AssetLoader {
public:
  AssetLoader() = default;
  Ref<Model> LoadModel(const std::string& path);
  Ref<Texture2D> LoadTexture(const std::string& path);

private:
  Ref<Model> LoadModelOBJ(const std::string& path);
  Ref<Model> LoadModelGLTF(const std::string& path);
  static std::string ExtractName(const std::string& path);
  static std::string ExtractExtension(const std::string& path);

  std::vector<Ref<Texture2D>> m_textureCache;
};

}  // namespace photon::util