#include "GUISystem.hpp"
#include "Common/Logging.hpp"

namespace photon::system {
static const char* glsl_version = "#version 450";
static const char* LightTypes[] = {"Point", "Directional"};
static int CurrentLightType     = 0;  // 0 Point; 1 Directional
Ref<GUISystem> GUISystem::Create(GLFWwindow* glfw_window) {
  return CreateRef<GUISystem>(glfw_window);
}

GUISystem::~GUISystem() {
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

GUISystem::GUISystem(GLFWwindow* glfw_window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

void GUISystem::Draw(Ref<RenderOptions> options) {
  BeginFrame();
  {
    ImGui::SetNextWindowSize(ImVec2(300, 300));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(0.5f);
    ImGui::Begin("GUI");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
    ImGui::Text("Frame time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::PopStyleColor();

    ImGui::Checkbox("Show Axis", &options->showAxis);
    ImGui::SameLine();
    ImGui::Checkbox("Show AABB", &options->showAABB);
    ImGui::Checkbox("Show Depth Debug", &options->showDepthDebug);

    //options->sceneChanged = ImGui::Combo("Select Model", &options->selectedModel,
    //                                     options->modelList, options->numModels);
    ImGui::Checkbox("Rotate Model", &options->rotateModel);
    ImGui::SameLine();
    ImGui::Checkbox("Rotate Camera", &options->rotateCamera);

    if (options->hasEnvMap && ImGui::CollapsingHeader("Environment")) {
      ImGui::Checkbox("Enable Environment Map", &options->enableEnvMap);
      ImGui::Checkbox("Show Background", &options->showBackground);
      ImGui::SameLine();
      ImGui::Checkbox("Blur", &options->blur);
    }
    if (options->lightType == LightType::Directional) {
      options->showLightModel = false;
    } else {
      options->showLightModel = true;
    }
    if (ImGui::CollapsingHeader("Scene Settings")) {
      ImGui::Checkbox("Show Floor", &options->showFloor);
      ImGui::Text("Light Type:");
      ImGui::RadioButton("Directional", reinterpret_cast<int*>(&options->lightType), 1);
      ImGui::RadioButton("Spot", reinterpret_cast<int*>(&options->lightType), 0);
      if (options->lightType == LightType::Spot) {
        ImGui::SameLine();
        ImGui::Checkbox("Rotate Light", &options->rotateLight);
      }
    }
    ImGui::End();
  }
  EndFrame();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUISystem::BeginFrame() {
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void GUISystem::EndFrame() {
  ImGui::EndFrame();
}

}  // namespace photon::system