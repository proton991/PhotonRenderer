#pragma once

#include "Assets/Model.hpp"
#include "Common/Base.hpp"
#include "RenderOption.hpp"
#include "Renderer/Skybox.hpp"

namespace photon {
namespace util {
class AssetCache;
}
class BaseScene;

class SceneBuilder {
public:
  template <typename T>
  static Ref<BaseScene> Create(const std::string& name) {
    return CreateRef<T>(name);
  }
};
class BaseScene {
public:
  friend class BasicRenderer;
  friend class ShadowMap;
  explicit BaseScene(std::string_view name);
  // disable copying
  BaseScene& operator=(const BaseScene&) = delete;
  BaseScene(const BaseScene&)            = delete;

  virtual void Init(){};
  virtual void LoadFloor(){};
  virtual void LoadLightModel(){};
  void AddModel(const std::string& model_name);
  void AddModel(const Ref<asset::Model>& model);
  virtual void LoadNewModel(uint32_t index) = 0;

  void Update(const Ref<RenderOptions>& options, float time = 0.0f);

  [[nodiscard]] AABB GetAABB() const;

  const auto GetLightPos() const { return m_lightModel->GetAABB().GetCenter(); }
  const auto& GetLightDir() const { return m_lightDir; }
  const auto& GetLightIntensity() const { return m_lightIntensity; }
  void SwitchLight();
  auto HasSkybox() const { return m_skybox != nullptr; }
  const auto& GetName() const { return m_name; }

  virtual int GetNumModels()          = 0;
  virtual const char** GetModelData() = 0;

protected:
  std::string m_name;
  Unique<util::AssetCache> m_assetCache;
  std::vector<Ref<asset::Model>> m_models;
  Ref<asset::Model> m_floor;
  Ref<asset::Model> m_lightModel;
  Ref<Skybox> m_skybox;
  glm::vec3 m_lightDir{-1.0f, -1.0f, -1.0f};
  glm::vec3 m_lightIntensity{1.0f};
  bool m_lightOn{true};
};
}  // namespace photon
