#include "framebuffer.hpp"
#include "Common/Logging.hpp"

namespace photon::gl {
AttachmentInfo AttachmentInfo::Color(std::string name_, AttachmentBinding binding_, int w, int h) {
  AttachmentInfo info{};
  info.type    = AttachmentType::TEXTURE_2D;
  info.name    = std::move(name_);
  info.binding = binding_;
  info.width   = w;
  info.height  = h;
  return info;
}

AttachmentInfo AttachmentInfo::Depth(int w, int h) {
  AttachmentInfo info{};
  info.type           = AttachmentType::TEXTURE_2D;
  info.name           = "depth";
  info.binding        = AttachmentBinding::DEPTH;
  info.width          = w;
  info.height         = h;
  info.minFilter      = GL_NEAREST;
  info.magFilter      = GL_NEAREST;
  info.wrap           = GL_CLAMP_TO_BORDER;
  info.internalFormat = GL_DEPTH_COMPONENT32F;
  return info;
}

Ref<Attachment> Attachment::Create(const AttachmentInfo& info) {
  return CreateRef<Attachment>(info);
}

Attachment::Attachment(const AttachmentInfo& info)
    : m_type(info.type), m_binding(info.binding), m_name(info.name) {
  if (m_type == AttachmentType::TEXTURE_2D) {
    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, info.minFilter);
    glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, info.magFilter);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, info.wrap);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, info.wrap);
    glTextureStorage2D(m_id, 1, info.internalFormat, info.width, info.height);
    if (info.generateMipmap) {
      glGenerateTextureMipmap(m_id);
    }
  } else if (m_type == AttachmentType::TEXTURE_2D_MS) {
    // default 4xMSAA
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, info.internalFormat, info.width,
                            info.height, GL_FALSE);
  } else if (m_type == AttachmentType::TEXTURE_CUBEMAP) {

    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_id);
    glTextureStorage2D(m_id,
                       info.level,           //level: number of  mipmaps
                       info.internalFormat,  // internal format
                       info.width, info.height);

    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, info.wrap);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, info.wrap);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, info.wrap);
    glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, info.minFilter);
    glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, info.magFilter);
    if (info.generateMipmap) {
      glGenerateTextureMipmap(m_id);
    }
  }
}

void Framebuffer::SetupDepthRbo() {
  glCreateRenderbuffers(1, &m_depthRBO);
  glNamedRenderbufferStorage(m_depthRBO, GL_DEPTH24_STENCIL8, m_width, m_height);
  glNamedFramebufferRenderbuffer(m_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthRBO);
}

Ref<Framebuffer> Framebuffer::Create(const FramebufferCreatInfo& info) {
  return CreateRef<Framebuffer>(info);
}

Framebuffer::Framebuffer(const FramebufferCreatInfo& info)
    : m_width(info.width), m_height(info.height), m_attachmentsInfos(info.attachmentsInfos) {
  Invalidate();
}

Framebuffer::~Framebuffer() {
  glDeleteFramebuffers(1, &m_id);
  glDeleteTextures(m_attachmentIds.size(), m_attachmentIds.data());
  m_attachments.clear();
  m_attachmentIds.clear();
}
void Framebuffer::SetupAttachments() {
  std::vector<GLenum> buffers;
  for (auto& attachmentInfo : m_attachmentsInfos) {
    // create attachment
    auto attachment = Attachment::Create(attachmentInfo);
    if (attachment->GetType() != AttachmentType::TEXTURE_CUBEMAP) {
      // cubemap texture need to be configured via attach_layer_texture
      glNamedFramebufferTexture(m_id, static_cast<int>(attachment->GetBinding()),
                                attachment->GetId(), 0);
    }
    m_attachmentIds.push_back(attachment->GetId());
    if (attachment->GetBinding() != AttachmentBinding::DEPTH &&
        attachment->GetBinding() != AttachmentBinding::DEPTH_STENCIL) {
      buffers.push_back(static_cast<GLenum>(attachment->GetBinding()));
    } else {
      m_hasDepthTexture = true;
    }
    m_attachments.try_emplace(attachment->GetName(), attachment);
  }
  if (buffers.empty()) {
    glNamedFramebufferDrawBuffer(m_id, GL_NONE);
  } else {
    glNamedFramebufferDrawBuffers(m_id, buffers.size(), buffers.data());
  }
}

void Framebuffer::Invalidate() {
  if (m_id != 0) {
    glDeleteFramebuffers(1, &m_id);
    glDeleteTextures(m_attachmentIds.size(), m_attachmentIds.data());
    m_attachments.clear();
    m_attachmentIds.clear();
  }
  glCreateFramebuffers(1, &m_id);
  SetupAttachments();
  if (!m_hasDepthTexture) {
    SetupDepthRbo();
  }
  auto status = glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    LOGE("Framebuffer is not complete, status {}", status);
  }
  Unbind();
}

void Framebuffer::AttachLayerTexture(int layer, const std::string& name, int level) {
  const auto& attachment = m_attachments.at(name);
  glNamedFramebufferTextureLayer(m_id, static_cast<int>(attachment->GetBinding()),
                                 attachment->GetId(), level, layer);
  glClearNamedFramebufferfv(m_id, GL_COLOR, 0, ClearColor);
  glClearNamedFramebufferfv(m_id, GL_DEPTH, 0, &ClearDepthValue);
}

void Framebuffer::ResizeAttachment(const std::string& name, int width, int height) {
  const auto& att = m_attachments.at(name);
  if (att->GetType() != AttachmentType::TEXTURE_CUBEMAP) {
    glTextureStorage2D(att->GetId(), 1, att->GetInternalFormat(), width, height);
    glNamedFramebufferTexture(m_id, static_cast<int>(att->GetBinding()), att->GetId(), 0);
  }
}

void Framebuffer::ResizeDepthRenderbuffer(int width, int height) {
  glNamedRenderbufferStorage(m_depthRBO, GL_DEPTH24_STENCIL8, width, height);
  glNamedFramebufferRenderbuffer(m_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthRBO);
}

void Framebuffer::AddAttachment(const AttachmentInfo& attachmentInfo) {
  auto attachment = Attachment::Create(attachmentInfo);
  glNamedFramebufferTexture(m_id, static_cast<int>(attachment->GetBinding()), attachment->GetId(),
                            0);
  m_attachmentIds.push_back(attachment->GetId());
  m_attachments.try_emplace(attachment->GetName(), attachment);
}

void Framebuffer::Unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::BindForWriting(bool set_view_port) const {
  glBindFramebuffer(GL_FRAMEBUFFER, m_id);
  if (set_view_port) {
    glViewport(0, 0, m_width, m_height);
  }
}

void Framebuffer::BindForReading(const std::string& name, int slot) const {
  const auto& attachment = m_attachments.at(name);
  glBindTextureUnit(slot, attachment->GetId());
}

void Framebuffer::Clear() {
  glClearNamedFramebufferfv(m_id, GL_COLOR, 0, ClearColor);
  glClearNamedFramebufferfv(m_id, GL_DEPTH, 0, &ClearDepthValue);
}

void Framebuffer::ClearDepth() {
  glClearNamedFramebufferfv(m_id, GL_DEPTH, 0, &ClearDepthValue);
}
}  // namespace photon::gl