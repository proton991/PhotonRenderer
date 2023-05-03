#include "ShaderCache.hpp"
#include <fstream>
#include "Common/Logging.hpp"

namespace photon::util {

bool compile_shader(GLuint id, const std::string& code) {
  GLint success{GL_FALSE};
  GLint log_len{-1};
  const char* shader_src = code.c_str();
  glShaderSource(id, 1, &shader_src, nullptr);
  glCompileShader(id);
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);

  if (success == GL_FALSE) {
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_len);
    std::vector<GLchar> info_log(log_len);
    glGetShaderInfoLog(id, log_len, nullptr, info_log.data());
    LOGE("Failed to compile shader: {}", std::string(info_log.begin(), info_log.end()));
  }

  return success == GL_TRUE;
}

bool link_program(GLuint id) {
  GLint success{GL_FALSE};
  GLint log_len{-1};

  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &success);

  if (success == GL_FALSE) {
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_len);
    std::vector<GLchar> info_log(log_len);
    glGetProgramInfoLog(id, log_len, nullptr, info_log.data());
    LOGE("Failed to link program: {}", std::string(info_log.begin(), info_log.end()));
    return false;
  }

  glValidateProgram(id);
  glGetProgramiv(id, GL_VALIDATE_STATUS, &success);

  if (success == GL_FALSE) {
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_len);
    std::vector<GLchar> info_log(log_len);
    glGetProgramInfoLog(id, log_len, nullptr, info_log.data());
    LOGE("Failed to valid program: {}", std::string(info_log.begin(), info_log.end()));
    return false;
  }
  return success == GL_TRUE;
}
std::string ShaderCache::LoadShaderSource(const std::string& path) {
  std::ifstream in(path, std::ios::in);
  in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  if (!in) {
    LOGE("Failed to load shader source: {}", path);
    return "";
  }
  return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

void ShaderCache::Init(const std::vector<gl::ShaderProgramCreateInfo>& createInfos) {
  for (const auto& info : createInfos) {
    LOGT("Building shader program {}", info.name);
    std::vector<GLuint> shader_ids;
    shader_ids.reserve(info.stages.size());
    bool success = true;
    for (const auto& stage : info.stages) {
      auto id{glCreateShader(gl::ShaderStage::Name2GL_ENUM.at(stage.type))};
      auto shader_code = LoadShaderSource(stage.filePath);
      if (shader_code.empty()) {
        continue;
      }
      if (!compile_shader(id, shader_code)) {
        success = false;
        break;
      }
      shader_ids.emplace_back(id);
    }

    if (!success) {
      for (const auto id : shader_ids) {
        glDeleteShader(id);
      }
      continue;
    }

    // link program and validate
    GLuint program_id{glCreateProgram()};
    for (const auto id : shader_ids) {
      glAttachShader(program_id, id);
    }
    if (!link_program(program_id)) {
      for (const auto id : shader_ids) {
        glDetachShader(program_id, id);
        glDeleteShader(id);
      }
      continue;
    }
    for (const auto id : shader_ids) {
      glDeleteShader(id);
    }
    m_shaders.try_emplace(info.name, CreateRef<gl::ShaderProgram>(info.name, program_id));
  }
}

Ref<gl::ShaderProgram> ShaderCache::RequestShader(const std::string& name) {
  if (m_shaders.find(name) == m_shaders.end()) {
    LOGE("Shader: {} not loaded!", name);
    return nullptr;
  }
  return m_shaders.at(name);
}

}  // namespace photon::util