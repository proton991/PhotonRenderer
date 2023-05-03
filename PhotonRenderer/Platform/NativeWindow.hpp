#pragma once

#include <memory>
#include "Graphics/Context.hpp"

struct GLFWwindow;
namespace photon::platform {
struct Extend2D {
  int width;
  int height;
};

struct WindowConfig {
  unsigned int width;
  unsigned int height;
  int resizable;
  const char* title;
  int major_version;
  int minor_version;
};

class Window {
public:
  static WindowConfig DefaultConfig();
  explicit Window(const WindowConfig& config);
  Window(Window&&)                 = delete;
  Window& operator=(Window&&)      = delete;
  Window(const Window&)            = delete;
  Window& operator=(const Window&) = delete;
  [[nodiscard]] GLFWwindow* Handle() const { return m_window; }

  void SetGLFWCallbacks();

  void WaitForFocus();

  void SwapBuffers() const;

  void EnableCursor() const;

  void DisableCursor() const;

  [[nodiscard]] Extend2D GetFramebufferSize() const;

  void Update();

  [[nodiscard]] auto ShouldClose() const { return m_data.shouldClose; }
  [[nodiscard]] auto ShouldResize() const { return m_data.shouldResize; }

  void Resize() { m_data.shouldResize = false; }

  [[nodiscard]] auto GetWidth() const { return m_data.width; }
  [[nodiscard]] auto GetHeight() const { return m_data.height; }
  [[nodiscard]] auto GetAspect() const { return float(m_data.width) / float(m_data.height); }

private:
  bool CenterWindow();
  void Destroy();

  GLFWwindow* m_window{nullptr};
  std::unique_ptr<gl::Context> m_context;

  struct WindowData {
    // size
    int width;
    int height;

    // status
    bool shouldClose{false};
    bool showCursor{true};
    bool shouldResize{false};
  } m_data;
};
}  // namespace photon::platform
