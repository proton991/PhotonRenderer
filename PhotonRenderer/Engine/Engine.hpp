#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Common/Base.hpp"
#include "RenderOption.hpp"

namespace photon::system {
class Camera;
}  // namespace photon::system

namespace photon::util {
class StopWatch;
}

namespace photon::platform {
class Window;
}

namespace photon {
class BasicRenderer;
class BaseScene;

class Engine {
public:
  Engine() = default;

  void Initialize(const std::string& activeScene);

  void Run();

private:
  void LoadScene(uint32_t index);

  Ref<util::StopWatch> m_stopWatch;
  Ref<platform::Window> m_window;
  Ref<BaseScene> m_scene;
  std::unordered_map<std::string, Ref<BaseScene>> m_sceneCache;

  Ref<system::Camera> m_camera;

  Ref<BasicRenderer> m_renderer;

  Ref<RenderOptions> m_options;
};
}  // namespace photon
