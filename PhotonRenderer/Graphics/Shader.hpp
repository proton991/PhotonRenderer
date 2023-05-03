#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include <glad/glad.h>
#include "Common/Base.hpp"

namespace photon::gl {
bool CompileShader(GLuint id, const std::string& code);
bool LinkProgram(GLuint id);

struct ShaderStage {
  ShaderStage() noexcept = default;

  ShaderStage(std::string path, std::string type)
      : filePath{std::move(path)}, type{std::move(type)} {}

  std::string filePath;
  std::string type;
  static std::unordered_map<std::string, int> Name2GL_ENUM;
};

struct ShaderProgramCreateInfo {
  const std::string name;
  const std::vector<ShaderStage> stages;
};

class ShaderProgram {
public:
  ShaderProgram(std::string name, GLuint id);
  ~ShaderProgram();

  ShaderProgram(ShaderProgram&& other) noexcept;

  ShaderProgram& Use();

  ShaderProgram& SetUniform(const std::string& name, int value);
  ShaderProgram& SetUniform(const std::string& name, float value);
  ShaderProgram& SetUniform(const std::string& name, const glm::vec2& value);
  ShaderProgram& SetUniform(const std::string& name, const glm::vec3& value);
  ShaderProgram& SetUniform(const std::string& name, const glm::vec4& value);
  ShaderProgram& SetUniform(const std::string& name, const glm::mat3x3& value);
  ShaderProgram& SetUniform(const std::string& name, const glm::mat4x4& value);

  ShaderProgram(const ShaderProgram&)            = delete;
  ShaderProgram& operator=(ShaderProgram&&)      = delete;
  ShaderProgram& operator=(const ShaderProgram&) = delete;

  auto GetLocation(const std::string& name) const { return m_uniforms.at(name); }

private:
  void GetUniforms();
  std::unordered_map<std::string, int> m_uniforms;  // <name, location>
  GLuint m_id{0};
  std::string m_name;
};

class ShaderProgramFactory {
public:
  //  static std::optional<ShaderProgram> create_shader_program(const ShaderProgramCreateInfo& info);
  static Ref<ShaderProgram> CreateShaderProgram(const ShaderProgramCreateInfo& info);
};
}  // namespace photon::gl
