
#include "context.hpp"
// clang-format off
#include <glad/glad.h>  // glad must be included before GLFW
#include <GLFW/glfw3.h>
// clang-format on
#include <string>
#include "Common/Logging.hpp"

namespace photon::gl {
static const std::unordered_map<GLenum, const char*> SourceEnumToString = {
    {GL_DEBUG_SOURCE_API, "API"},
    {GL_DEBUG_SOURCE_WINDOW_SYSTEM, "WINDOW_SYSTEM"},
    {GL_DEBUG_SOURCE_SHADER_COMPILER, "SHADER_COMPILER"},
    {GL_DEBUG_SOURCE_THIRD_PARTY, "THIRD_PARTY"},
    {GL_DEBUG_SOURCE_APPLICATION, "APPLICATION"},
    {GL_DEBUG_SOURCE_OTHER, "OTHER"}};

static const std::unordered_map<GLenum, const char*> TypeEnumToString = {
    {GL_DEBUG_TYPE_ERROR, "ERROR"},
    {GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR"},
    {GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED_BEHAVIOR"},
    {GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY"},
    {GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE"},
    {GL_DEBUG_TYPE_OTHER, "OTHER"}};

static const std::unordered_map<GLenum, const char*> SeverityEnumToString = {
    {GL_DEBUG_SEVERITY_HIGH, "HIGH"},
    {GL_DEBUG_SEVERITY_MEDIUM, "MEDIUM"},
    {GL_DEBUG_SEVERITY_LOW, "LOW"},
    {GL_DEBUG_SEVERITY_NOTIFICATION, "NOTIFICATION"}};

// List of message type to ignore for GL Debug Output
static const std::vector<std::tuple<GLenum, GLenum, GLenum>> IgnoreList = {
    std::make_tuple(GL_DONT_CARE, GL_DONT_CARE,
                    GL_DEBUG_SEVERITY_NOTIFICATION)  // Ignore all notifications
};
void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                   const GLchar* message, GLvoid* userParam);
Context::Context(GLFWwindow* windowHandle) {
  glfwMakeContextCurrent(windowHandle);
  int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  if (!status) {
    LOGE("Failed to initialize glad!");
  }
  LOGT("Displaying OpenGL Info:");
  LOGI("Vendor: {}", std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR))));
  LOGI("Renderer: {}", std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER))));
  LOGI("Version: {}", std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
  InitDebug();
}

void Context::InitDebug() {
  glDebugMessageCallback((GLDEBUGPROCARB)DebugCallback, nullptr);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

  for (const auto& tuple : IgnoreList) {
    glDebugMessageControl(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple), 0, nullptr,
                          GL_FALSE);
  }
}

void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                   const GLchar* message, GLvoid* userParam) {
  const auto find_str = [](GLenum value, const auto& map) {
    const auto it = map.find(value);
    if (it == end(map)) {
      return "UNDEFINED";
    }
    return (*it).second;
  };

  const auto sourceStr   = find_str(source, SourceEnumToString);
  const auto typeStr     = find_str(type, TypeEnumToString);
  const auto severityStr = find_str(severity, SeverityEnumToString);
  LOGD("[GLDebug]: [source={} type={} severity{} id={}]", sourceStr, typeStr, severityStr, id);
}

}  // namespace photon::gl