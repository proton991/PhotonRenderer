#pragma once
#include <glad/glad.h>
#include <string>
#include "Common/Base.hpp"

namespace photon::asset {
struct TextureInfo {
  int width{0};
  int height{0};
  int minFilter{GL_LINEAR};
  int magFilter{GL_LINEAR};
  int wrapS{GL_REPEAT};
  int wrapT{GL_REPEAT};
  int wrapR{GL_REPEAT};
  bool generateMipmap{false};
  GLenum internalFormat{GL_RGBA8};
  GLenum dataFormat{GL_RGBA};
  GLenum dataType{GL_UNSIGNED_BYTE};
};
class Texture2D {
public:
  static Ref<Texture2D> Create(const TextureInfo& info, const void* data) {
    return CreateRef<Texture2D>(info, data);
  }

  static Ref<Texture2D> CreateDefaultWhite();

  explicit Texture2D(const std::string& path);
  Texture2D(const TextureInfo& info, const void* data);
  ~Texture2D();

  void Bind(GLenum slot) const;

  uint32_t GetId() const { return m_id; }

  GLuint64 GetHandle() const { return m_handle; }

private:
  uint32_t m_id{0};
  GLuint64 m_handle{0};
  int m_width;
  int m_height;
  GLenum m_internalFormat, m_dataFormat;
};

class TextureCubeMap {
public:
  static Ref<TextureCubeMap> Create(const TextureInfo& info,
                                    std::array<unsigned char*, 6> faceData);
  TextureCubeMap(const TextureInfo& info, const std::array<unsigned char*, 6>& faceData);
  ~TextureCubeMap();
  void Bind(GLenum slot) const;

private:
  uint32_t m_id{0};
};
}  // namespace photon::asset
