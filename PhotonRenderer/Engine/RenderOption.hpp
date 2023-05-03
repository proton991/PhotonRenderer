#pragma once

namespace photon {
enum class LightType : decltype(0) { Spot = 0, Directional = 1 };
struct RenderOptions {
  RenderOptions() = default;
  const char** modelList{nullptr};
  uint32_t numModels{0};
  int selectedModel{0};
  bool rotateModel{false};
  bool rotateLight{false};
  bool rotateCamera{false};
  bool hasEnvMap{true};
  bool enableEnvMap{true};
  bool showBackground{true};
  bool showAxis{false};
  bool showAABB{false};
  bool showFloor{true};
  bool showLightModel{true};
  bool blur{false};
  bool sceneChanged{false};
  bool showDepthDebug{false};
  LightType lightType{LightType::Directional};
};
}  // namespace photon
