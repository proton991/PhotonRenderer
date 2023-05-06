#include "SimpleScene.hpp"
#include <glm/gtx/string_cast.hpp>
#include "Renderer/Skybox.hpp"
#include "Utils/AssetCache.hpp"

namespace photon {
void SimpleScene::Init() {
  // setup skybox
  m_skybox = Skybox::Create("Data/Textures/sky.hdr", 2048);
  GenerateTerrain();
  m_center = m_terrain->GetCenter();

  auto starship1 = m_assetCache->RequestModel("Data/Models/Starship/Starship.obj");
  auto ufo       = m_assetCache->RequestModel("Data/Models/ufo/scene.gltf");

  const auto modelSize = starship1->GetAABB().GetSize();
  starship1->Translate({m_center.x, m_center.y * 1.1f, m_center.z});
  AddModel(starship1);

  ufo->Scale(modelSize / ufo->GetAABB().GetSize());
  ufo->Translate({m_center.x, m_center.y * 1.1f + modelSize, m_center.z});
  AddModel(ufo);
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

  m_lightModel->Translate({1.8f * sceneBound.x, 1.5f * sceneBound.y, 1.8f * sceneBound.z});
  m_assetCache->RemoveModel(m_lightModel->GetName());
}

void SimpleScene::GenerateTerrain() {
  TerrainCreateInfo info{};
  info.maxHeigt     = 356.0;
  info.minHeight    = 0.0f;
  info.roughness    = 1.0f;
  info.terrainSize  = 513;
  info.patchSize    = 33;
  info.wordScale    = 2.0f;
  info.textureScale = 4.0f;
  info.textureFiles = {"Data/Textures/ground1.jpg", "Data/Textures/ground2.jpg",
                       "Data/Textures/grass.png", "Data/Textures/water.png"};

  m_terrain = CreateRef<Terrain>(info);
  m_terrain->Init();
}

void SimpleScene::LoadFloor() {
  m_floor = m_assetCache->RequestModel(FloorPath);

  const auto aabb         = GetAABB();
  const auto scene_size   = glm::length(aabb.diag);
  const auto floor_size   = glm::length(m_floor->GetAABB().diag);
  const auto scale_factor = 10.0f * scene_size / floor_size;
  m_floor->Scale(scale_factor);
  m_floor->Translate(glm::vec3(m_center.x, aabb.posMin.y * 0.99f, m_center.z));
  m_assetCache->RemoveModel(m_floor->GetName());
}
}  // namespace photon
