#pragma once

struct GLFWwindow;
namespace photon::gl {

class Context {
public:
  explicit Context(GLFWwindow* windowHandle);

  Context(Context&&)                 = delete;
  Context(const Context&)            = delete;
  Context& operator=(Context&&)      = delete;
  Context& operator=(const Context&) = delete;

private:
  void InitDebug();
};
}  // namespace photon::gl
