#ifndef EASYGRAPHICS_GUI_SYSTEM_HPP
#define EASYGRAPHICS_GUI_SYSTEM_HPP
#include <string>
#include <vector>
#include "Common/Base.hpp"
#include "Engine/RenderOption.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
namespace photon::system {
class GUISystem {
  friend class Engine;

public:
  static Ref<GUISystem> Create(GLFWwindow* glfw_window);
  GUISystem(GLFWwindow* glfw_window);
  ~GUISystem();

  void Draw(Ref<RenderOptions> options);

private:
  void BeginFrame();
  void EndFrame();
};
}  // namespace photon::system
#endif  //EASYGRAPHICS_GUI_SYSTEM_HPP
