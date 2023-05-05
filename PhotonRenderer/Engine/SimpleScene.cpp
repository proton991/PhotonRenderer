#include "SimpleScene.hpp"
#include <glm/gtx/string_cast.hpp>
#include "Renderer/Skybox.hpp"
#include "Utils/AssetCache.hpp"

namespace photon {
void SimpleScene::Init() {
  // setup skybox
  m_skybox = Skybox::Create("Data/Textures/sky.hdr", 2048);

  auto starship1 = m_assetCache->RequestModel("Data/Models/Starship/Starship.obj");

  const auto modelSize = starship1->GetAABB().GetSize();
  starship1->Translate({0, 0, 0});
  AddModel(starship1);
  auto starship2 = m_assetCache->RequestModel("Data/Models/ufo/scene.gltf");
  starship2->Scale(modelSize / starship2->GetAABB().GetSize());
  starship2->Translate({0.0, modelSize, 0.0});
  AddModel(starship2);
  LoadFloor();
  LoadLightModel();
}

void SimpleScene::LoadNewModel(uint32_t index) {
  m_models.clear();
  AddModel(ModelPaths[index]);
}

void SimpleScene::LoadLightModel() {
  m_lightModel = m_assetCache->RequestModel(LightModelPath);

  const auto aabb        = GetAABB();
  const auto scene_size  = glm::length(aabb.diag);
  const auto light_size  = glm::length(m_lightModel->GetAABB().diag);
  const auto scaleFactor = scene_size / light_size;

  m_lightModel->Scale(2 * scaleFactor);
  auto sceneBound = aabb.posMax;

  m_lightModel->Translate({sceneBound.x * 5.0f, sceneBound.y * 3.0f, sceneBound.z * 5.0f});
  m_assetCache->RemoveModel(m_lightModel->GetName());
}

void SimpleScene::LoadFloor() {
  m_floor = m_assetCache->RequestModel(FloorPath);

  const auto aabb         = GetAABB();
  const auto scene_size   = glm::length(aabb.diag);
  const auto floor_size   = glm::length(m_floor->GetAABB().diag);
  const auto scale_factor = 10.0f * scene_size / floor_size;
  m_floor->Scale(scale_factor);
  m_floor->Translate(glm::vec3(0.0, aabb.posMin.y * 1.01, 0.0));
  m_assetCache->RemoveModel(m_floor->GetName());
}
}  // namespace photon
