#pragma once

#include "Engine/RenderOption.hpp"
#include "Engine/Scene.hpp"
#include "Systems/CameraSystem.hpp"

namespace photon {
class ShaderProgram;
class Model;
class BaseScene;

struct FrameInfo {
  const Ref<BaseScene>& scene;
  const Ref<RenderOptions>& options;
  const Ref<system::Camera>& camera;
};
}  // namespace photon
