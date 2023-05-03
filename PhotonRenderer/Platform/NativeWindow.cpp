#include "NativeWindow.hpp"
#include <GLFW/glfw3.h>
#include "Common/Logging.hpp"
#include "NativeInput.hpp"

namespace photon::platform {

static void glfw_error_call_back(int code, const char* msg) {
  LOGE("GLFW error [{}]: {}", code, msg);
}

WindowConfig Window::DefaultConfig() {
  WindowConfig config{};
  config.width         = 800;
  config.height        = 800;
  config.major_version = 4;
  config.minor_version = 5;
  config.resizable     = GL_FALSE;
  config.title         = "OpenGL Renderer";

  return config;
}

void Window::SetGLFWCallbacks() {
  const auto resize_callback = [](GLFWwindow* w, int width, int height) {
    auto* window_data         = static_cast<WindowData*>(glfwGetWindowUserPointer(w));
    window_data->width        = width;
    window_data->height       = height;
    window_data->shouldResize = true;
    LOGT("Window resized to {} x {}", width, height);
  };
  glfwSetWindowSizeCallback(m_window, resize_callback);

  const auto key_callback = [](GLFWwindow* w, auto key, auto scancode, auto action, auto mode) {
    if (key < 0 || key > GLFW_KEY_LAST) {
      return;
    }
    switch (action) {
      case GLFW_PRESS:
        KeyboardMouseInput::GetInstance().PressKey(key);
        break;
      case GLFW_RELEASE:
        KeyboardMouseInput::GetInstance().ReleaseKey(key);
        break;
      default:
        break;
    }
  };
  glfwSetKeyCallback(m_window, key_callback);

  const auto cursor_pos_callback = [](GLFWwindow* w, auto xPos, auto yPos) {
    KeyboardMouseInput::GetInstance().SetCursorPos(xPos, yPos);
  };
  glfwSetCursorPosCallback(m_window, cursor_pos_callback);

  auto mouse_button_callback = [](GLFWwindow* window, int button, int action, int mods) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
      return;
    }
    switch (action) {
      case GLFW_PRESS:
        KeyboardMouseInput::GetInstance().PressMouseButton(button);
        break;
      case GLFW_RELEASE:
        KeyboardMouseInput::GetInstance().ReleaseMouseButton(button);
        break;
      default:
        break;
    }
  };
  glfwSetMouseButtonCallback(m_window, mouse_button_callback);
}

Window::Window(const WindowConfig& config) {
  m_data.width  = config.width;
  m_data.height = config.height;

  if (!glfwInit()) {
    LOGE("Failed to initialize GLFW!");
    abort();
  }
  glfwSetErrorCallback(glfw_error_call_back);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.major_version);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.minor_version);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, config.resizable);

  m_window = glfwCreateWindow(m_data.width, m_data.height, config.title, nullptr, nullptr);

  if (!m_window) {
    LOGE("Failed to create GLFW window!");
    abort();
  }
  // initialize context
  m_context = std::make_unique<gl::Context>(m_window);

  glfwSetWindowUserPointer(m_window, &m_data);

  CenterWindow();
  // setup input callbacks
  SetGLFWCallbacks();
}

void Window::WaitForFocus() {
  int current_width  = 0;
  int current_height = 0;

  do {
    glfwWaitEvents();
    glfwGetFramebufferSize(m_window, &current_width, &current_height);
  } while (current_width == 0 || current_height == 0);

  m_data.width  = current_width;
  m_data.height = current_height;
}

void Window::SwapBuffers() const {
  glfwSwapBuffers(m_window);
}

void Window::EnableCursor() const {
  glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::DisableCursor() const {
  glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Extend2D Window::GetFramebufferSize() const {
  return {m_data.width, m_data.height};
}
void Window::Update() {
  glfwPollEvents();
  if (KeyboardMouseInput::GetInstance().IsKeyPressed(GLFW_KEY_TAB)) {
    m_data.showCursor = !m_data.showCursor;
    if (m_data.showCursor) {
      EnableCursor();
    } else {
      DisableCursor();
    }
  }
  if (KeyboardMouseInput::GetInstance().IsKeyPressed(GLFW_KEY_ESCAPE) ||
      glfwWindowShouldClose(m_window)) {
    m_data.shouldClose = true;
    glfwSetWindowShouldClose(m_window, GL_TRUE);
  }
}

bool Window::CenterWindow() {
  int sx = 0, sy = 0;
  int px = 0, py = 0;
  int mx = 0, my = 0;
  int monitor_count = 0;
  int best_area     = 0;
  int final_x = 0, final_y = 0;

  glfwGetWindowSize(m_window, &sx, &sy);
  glfwGetWindowPos(m_window, &px, &py);

  // Iterate throug all monitors
  GLFWmonitor** m = glfwGetMonitors(&monitor_count);
  if (!m)
    return false;

  for (int j = 0; j < monitor_count; ++j) {

    glfwGetMonitorPos(m[j], &mx, &my);
    const GLFWvidmode* mode = glfwGetVideoMode(m[j]);
    if (!mode)
      continue;

    // Get intersection of two rectangles - screen and window
    int minX = std::max(mx, px);
    int minY = std::max(my, py);

    int maxX = std::min(mx + mode->width, px + sx);
    int maxY = std::min(my + mode->height, py + sy);

    // Calculate area of the intersection
    int area = std::max(maxX - minX, 0) * std::max(maxY - minY, 0);

    // If its bigger than actual (window covers more space on this monitor)
    if (area > best_area) {
      // Calculate proper position in this monitor
      final_x = mx + (mode->width - sx) / 2;
      final_y = my + (mode->height - sy) / 2;

      best_area = area;
    }
  }

  // We found something
  if (best_area)
    glfwSetWindowPos(m_window, final_x, final_y);

  // Something is wrong - current window has NOT any intersection with any monitors. Move it to the default one.
  else {
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    if (primary) {
      const GLFWvidmode* desktop = glfwGetVideoMode(primary);

      if (desktop)
        glfwSetWindowPos(m_window, (desktop->width - sx) / 2, (desktop->height - sy) / 2);
      else
        return false;
    } else
      return false;
  }

  return true;
}

void Window::Destroy() {
  glfwDestroyWindow(m_window);
  glfwTerminate();
}
}  // namespace photon::platform