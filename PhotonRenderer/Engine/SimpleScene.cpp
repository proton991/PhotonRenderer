#include "SimpleScene.hpp"
#include <glm/gtx/string_cast.hpp>
#include "Renderer/Skybox.hpp"
#include "Utils/AssetCache.hpp"

namespace photon {
void SimpleScene::Init() {
  // setup skybox
  m_skybox = Skybox::Create("Data/Textures/barcelona.hdr", 2048);
  AddModel(ModelPaths[0]);
  LoadFloor();
  LoadLightModel();
}

void SimpleScene::LoadNewModel(uint32_t index) {
  m_models.clear();
  AddModel(ModelPaths[index]);
}

void SimpleScene::LoadLightModel() {
  m_lightModel = m_assetCache->RequestModel(LightModelPath);

  const auto aabb         = GetAABB();
  const auto scene_size   = glm::length(aabb.diag);
  const auto light_size   = glm::length(m_lightModel->GetAABB().diag);
  const auto scale_factor = scene_size / light_size;

  m_lightModel->Scale(2 * scale_factor);
  m_lightModel->Translate(aabb.posMax * 5.0f);
  m_assetCache->RemoveModel(m_lightModel->GetName());
}

void SimpleScene::LoadFloor() {
  m_floor = m_assetCache->RequestModel(FloorPath);

  const auto aabb         = GetAABB();
  const auto scene_size   = glm::length(aabb.diag);
  const auto floor_size   = glm::length(m_floor->GetAABB().diag);
  const auto scale_factor = 3 * scene_size / floor_size;
  m_floor->Scale(scale_factor);
  m_floor->Translate(glm::vec3(0.0, aabb.posMin.y * 1.01, 0.0));
  m_assetCache->RemoveModel(m_floor->GetName());
}
}  // namespace photon
