#pragma once
#include <string>
#include <unordered_map>
#include "Graphics/Shader.hpp"

namespace photon::util {
bool compile_shader(GLuint id, const std::string& code);
bool link_program(GLuint id);
class ShaderCache {
public:
  void Init(const std::vector<gl::ShaderProgramCreateInfo>& createInfos);
  Ref<gl::ShaderProgram> RequestShader(const std::string& name);
  static std::string LoadShaderSource(const std::string& path);

private:
  std::unordered_map<std::string, Ref<gl::ShaderProgram>> m_shaders;
};
}  // namespace photon::util