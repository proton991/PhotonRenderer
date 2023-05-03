// clang-format off
#include "Engine.hpp"
#include "Common/Logging.hpp"
#include "Platform/NativeWindow.hpp"
#include "Renderer/BasicRenderer.hpp"
#include "SimpleScene.hpp"
#include "Utils/StopWatch.hpp"
#include "Utils/AssetCache.hpp"
#include "Platform/NativeInput.hpp"  // include glfw after glad
using namespace photon::system;
using namespace photon::platform;
// clang-format on
namespace photon {

void Engine::Initialize(const std::string& activeScene) {
  // setup window
  WindowConfig config{};
  config.width         = 900;
  config.height        = 900;
  config.major_version = 4;
  config.minor_version = 5;
  config.resizable     = GL_TRUE;
  config.title         = "OpenGL Renderer";
  m_window             = CreateRef<Window>(config);

  m_options = CreateRef<RenderOptions>();

  // setup renderer

  RendererConfig render_config{
      config.width,
      config.height,
  };
  m_renderer = CreateRef<BasicRenderer>(render_config);

  // setup scenes
  auto simpleScene = CreateRef<SimpleScene>("SimpleScene");
  simpleScene->Init();

  m_sceneCache.try_emplace(simpleScene->GetName(), std::move(simpleScene));
  LOGI("Activate scene: {}", activeScene);
  m_scene = m_sceneCache.at(activeScene);
  if (!m_scene->HasSkybox()) {
    m_options->hasEnvMap = false;
  }

  // setup camera
  auto aabb = m_scene->GetAABB();
  m_camera  = Camera::Create(aabb.posMin, aabb.posMax, m_window->GetAspect());

  // timer
  m_stopWatch = CreateRef<util::StopWatch>();
}

void Engine::LoadScene(uint32_t index) {
  m_scene->LoadNewModel(index);
  m_scene->LoadFloor();
  m_scene->LoadLightModel();
  auto aabb = m_scene->GetAABB();
  m_camera  = Camera::Create(aabb.posMin, aabb.posMax, m_window->GetAspect());
}

void Engine::Run() {
  m_options->numModels = m_scene->GetNumModels();
  m_options->modelList = m_scene->GetModelData();
  while (!m_window->ShouldClose()) {
    FrameInfo frameInfo{m_scene, m_options, m_camera};
    if (m_window->ShouldResize()) {
      m_window->Resize();
      m_renderer->ResizeFbos(m_window->GetWidth(), m_window->GetHeight());
      float aspect = (float)m_window->GetWidth() / (float)m_window->GetHeight();
      m_camera->UpdateAspect(aspect);
    }
    float deltaTime = m_stopWatch->TimeStep();

    m_scene->Update(m_options, deltaTime);

    m_camera->Update(deltaTime, m_options->rotateCamera);

    m_renderer->RenderFrame(frameInfo);

    if (m_options->sceneChanged) {
      LoadScene(m_options->selectedModel);
    }
    m_window->SwapBuffers();
    m_window->Update();
  }
}
}  // namespace photon