#include "Scene.hpp"
#include "Common/Logging.hpp"
#include "Platform/NativeInput.hpp"
#include "Utils/AssetCache.hpp"

namespace photon {
BaseScene::BaseScene(std::string_view name) : m_name(name) {
  LOGI("Loading scene: {}", m_name);
  m_assetCache = CreateUnique<util::AssetCache>();
}

void BaseScene::AddModel(const Ref<asset::Model>& model) {
  m_models.push_back(model);
}

void BaseScene::AddModel(const std::string& model_path) {
  m_models.push_back(m_assetCache->RequestModel(model_path));
}

void BaseScene::Update(const Ref<RenderOptions>& options, float time) {
  // turn on/off light
  if (platform::KeyboardMouseInput::GetInstance().WasKeyPressedOnce(GLFW_KEY_L)) {
    SwitchLight();
  }
  float angle = time * 0.5f;
  if (options->rotateModel) {
    for (auto& model : m_models) {
      model->Rotate(angle);
    }
  }
  if (options->rotateLight) {
    const auto point = glm::vec3(0, m_lightModel->GetAABB().GetCenter().y, 0);
    m_lightModel->Rotate(angle, point);
  }
}

AABB BaseScene::GetAABB() const {
  AABB aabb = m_models[0]->GetAABB();
  for (int i = 1; i < m_models.size(); i++) {
    aabb.posMax = glm::max(m_models[i]->GetAABB().posMax, aabb.posMax);
    aabb.posMin = glm::min(m_models[i]->GetAABB().posMin, aabb.posMin);
  }
  return aabb;
}

void BaseScene::SwitchLight() {
  m_lightOn = !m_lightOn;
  if (!m_lightOn) {
    m_lightIntensity = glm::vec3(0.0f);
    LOGI("light off");
  } else {
    m_lightIntensity = glm::vec3(1.0f);
    LOGI("light on");
  }
}
}  // namespace photon