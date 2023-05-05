#pragma once

#include "Scene.hpp"

namespace photon {
class SimpleScene : public BaseScene {
public:
  explicit SimpleScene(std::string_view name) : BaseScene(name) {}
  void LoadNewModel(uint32_t index) override;
  void LoadFloor() override;
  void LoadLightModel() override;
  void Init() override;

  int GetNumModels() override { return ModelNames.size(); }
  const char** GetModelData() override { return ModelNames.data(); }

private:
  const char* FloorPath{"Data/Models/wood_floor/scene.gltf"};
  const char* LightModelPath{"Data/Models/sun/scene.gltf"};
  std::vector<const char*> ModelNames{"Starship"};

  std::vector<const char*> ModelPaths{
      "Data/Models/Starship/Starship.obj",
  };
};
}  // namespace photon
