#include "Texture.hpp"
#include <stb_image.h>
#include "Common/Logging.hpp"

namespace photon::asset {
Texture2D::Texture2D(const std::string& path) {
  m_internalFormat = GL_RGBA8;
  m_dataFormat     = GL_RGBA;

  int width, height, channels;
  //  stbi_set_flip_vertically_on_load(1);
  stbi_uc* data = nullptr;

  data = stbi_load(path.c_str(), &width, &height, &channels, 0);
  if (data) {
    m_width  = width;
    m_height = height;
    if (channels == 4) {
      m_internalFormat = GL_RGBA8;
      m_dataFormat     = GL_RGBA;
    } else if (channels == 3) {
      m_internalFormat = GL_RGB8;
      m_dataFormat     = GL_RGB;
    }
    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
    glTextureStorage2D(m_id, 1, m_internalFormat, m_width, m_height);

    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
    //    glGenerateTextureMipmap(m_id);

    stbi_image_free(data);
  } else {
    spdlog::error("Failed to load texture {}", path);
  }
}

Texture2D::Texture2D(const TextureInfo& info, const void* data) {
  m_internalFormat = info.internalFormat;
  m_dataFormat     = info.dataFormat;
  m_width          = info.width;
  m_height         = info.height;

  glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
  glTextureStorage2D(m_id, 1, m_internalFormat, m_width, m_height);

  glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, info.wrapS);
  glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, info.wrapT);
  //  glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, info.wrap_r);
  glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, info.minFilter);
  glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, info.magFilter);

  glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, m_dataFormat, info.dataType, data);
  if (info.generateMipmap) {
    glGenerateTextureMipmap(m_id);
  }

  m_handle = glGetTextureHandleARB(m_id);
  glMakeTextureHandleResidentARB(m_handle);
}

Ref<Texture2D> Texture2D::CreateDefaultWhite() {
  return std::make_shared<Texture2D>("Data/Textures/white.png");
}

Texture2D::~Texture2D() {
  if (m_handle != 0) {
    glMakeTextureHandleNonResidentARB(m_handle);
    m_handle = 0;
  }
  if (m_id != 0) {
    glDeleteTextures(1, &m_id);
  }
}
void Texture2D::Bind(GLenum slot) const {
  glBindTextureUnit(slot, m_id);
}

Ref<TextureCubeMap> TextureCubeMap::Create(const TextureInfo& info,
                                           std::array<unsigned char*, 6> faceData) {
  return CreateRef<TextureCubeMap>(info, faceData);
}

TextureCubeMap::TextureCubeMap(const TextureInfo& info,
                               const std::array<unsigned char*, 6>& faceData) {
  glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_id);
  glTextureStorage2D(m_id,
                     1,                    // one level, no mipmaps
                     info.internalFormat,  // internal format
                     info.width, info.height);

  glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, info.wrapS);
  glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, info.wrapT);
  glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, info.wrapR);
  glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, info.minFilter);
  glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, info.magFilter);

  for (int face = 0; face < 6; ++face) {
    // face:
    // 0 = positive x face
    // 1 = negative x face
    // 2 = positive y face
    // 3 = negative y face
    // 4 = positive z face
    // 5 = negative z face

    glTextureSubImage3D(
        m_id,
        0,     // only 1 level in example
        0,     // x offset
        0,     // y offset
        face,  // the offset to desired cubemap face, which offset goes to which face above
        info.width, info.height,
        1,  // depth how many faces to set, if this was 3 we'd set 3 cubemap faces at once
        info.dataFormat, info.dataType, faceData[face]);
  }
}

void TextureCubeMap::Bind(GLenum slot) const {
  glBindTextureUnit(slot, m_id);
}

TextureCubeMap::~TextureCubeMap() {
  if (m_id != 0) {
    glDeleteTextures(1, &m_id);
  }
}
}  // namespace photon::asset