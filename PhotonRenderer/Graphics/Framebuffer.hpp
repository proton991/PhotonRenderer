#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "Common/Base.hpp"
#include "Graphics/UniformBuffer.hpp"
#include "Graphics/VertexArray.hpp"

namespace photon::gl {
enum class AttachmentType { TEXTURE_2D, TEXTURE_2D_MS, TEXTURE_CUBEMAP, RENDER_BUFFER };

enum class AttachmentBinding : decltype(GL_COLOR_ATTACHMENT0) {
  COLOR0 = GL_COLOR_ATTACHMENT0,
  COLOR1 = GL_COLOR_ATTACHMENT1,
  COLOR2 = GL_COLOR_ATTACHMENT2,
  COLOR3 = GL_COLOR_ATTACHMENT3,
  COLOR4 = GL_COLOR_ATTACHMENT4,

  DEPTH_STENCIL = GL_DEPTH_STENCIL_ATTACHMENT,
  DEPTH         = GL_DEPTH_ATTACHMENT,
};

struct AttachmentInfo {
  static AttachmentInfo Color(std::string name_, AttachmentBinding binding_, int w, int h);
  static AttachmentInfo Depth(int w, int h);
  int width{0};
  int height{0};
  int level{1};
  AttachmentType type;
  AttachmentBinding binding;
  std::string name;
  // having default value
  int wrap{GL_CLAMP_TO_EDGE};
  int magFilter{GL_LINEAR};
  int minFilter{GL_LINEAR};
  GLenum internalFormat{GL_RGBA8};
  bool generateMipmap{false};
};

class Attachment {
public:
  static Ref<Attachment> Create(const AttachmentInfo& info);

  Attachment(const AttachmentInfo& info);

  [[nodiscard]] auto GetType() const { return m_type; }
  [[nodiscard]] auto GetId() const { return m_id; }
  [[nodiscard]] auto GetBinding() const { return m_binding; }
  [[nodiscard]] const auto& GetName() const { return m_name; }
  [[nodiscard]] const auto& GetInternalFormat() const { return m_internal_format; }

private:
  uint32_t m_id{0};
  AttachmentType m_type;
  AttachmentBinding m_binding;
  std::string m_name;
  GLenum m_internal_format;
};
struct FramebufferCreatInfo {
  uint32_t width{800};
  uint32_t height{800};
  std::vector<AttachmentInfo> attachmentsInfos;
};
class Framebuffer {
public:
  static Ref<Framebuffer> Create(const FramebufferCreatInfo& info);
  Framebuffer(const FramebufferCreatInfo& info);
  ~Framebuffer();
  void BindForWriting(bool set_view_port = true) const;
  void Unbind() const;

  void BindForReading(const std::string& name, int slot) const;
  void AttachLayerTexture(int layer, const std::string& name, int level = 0);
  void AddAttachment(const AttachmentInfo& attachmentInfo);

  void ResizeAttachment(const std::string& name, int width, int height);
  void ResizeDepthRenderbuffer(int width, int height);

  void Clear();
  void ClearDepth();

  const auto GetTextureId(const std::string& name) const {
    return m_attachments.at(name)->GetId();
  }

private:
  void SetupDepthRbo();
  void SetupAttachments();
  void Invalidate();

  uint32_t m_id{0};
  int m_width;
  int m_height;
  std::vector<AttachmentInfo> m_attachmentsInfos;
  std::unordered_map<std::string, Ref<Attachment>> m_attachments;
  std::vector<uint32_t> m_attachmentIds;
  uint32_t m_depthRBO{0};

  const float ClearColor[4]   = {0.0f, 0.0f, 0.0f, 1.0f};
  const float ClearDepthValue = 1.0f;
  bool m_hasDepthTexture{false};
};
}  // namespace photon::gl
