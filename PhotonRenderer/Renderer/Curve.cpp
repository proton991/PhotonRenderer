#include "Curve.hpp"
#include "RenderAPI.hpp"
#include "Systems/CameraSystem.hpp"
#include "Vertex.hpp"
namespace photon {
bool compareVertices(const LineVertex& v1, const LineVertex& v2) {
  return glm::length(v1.position - v2.position) < 1e-6f;
}
Curve::Curve() {
  gl::ShaderProgramCreateInfo shaderInfo{"CurveShader",
                                         {
                                             {"Data/Shaders/lines.vs.glsl", "vertex"},
                                             {"Data/Shaders/lines.fs.glsl", "fragment"},
                                         }};
  m_shader = gl::ShaderProgramFactory::CreateShaderProgram(shaderInfo);
  m_vao    = gl::VertexArray::Create();
  std::vector<LineVertex> vertices;
  const auto color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
  const auto delta = 0.01f;

  auto prevPos = ControlPoints[0];
  for (auto t = 0.0f; t < 1.0f; t += delta) {
    glm::vec3 pos = ControlPoints[0] * (1 - t) * (1 - t) * (1 - t) +  //
                    ControlPoints[1] * (1 - t) * (1 - t) * t +        //
                    ControlPoints[2] * (1 - t) * t * t +              //
                    ControlPoints[3] * t * t * t;                     //
    vertices.push_back({prevPos, color});
    vertices.push_back({pos, color});
    prevPos = pos;
  }
  m_numPoints = vertices.size();
  auto vbo    = gl::VertexBuffer::Create(vertices.size() * sizeof(LineVertex), vertices.data());
  vbo->SetBufferView({
      {"aPosition", gl::BufferDataType::Vec3f},
      {"aColor", gl::BufferDataType::Vec4f},
  });
  m_vao->Bind();
  m_vao->AttachVertexBuffer(vbo);
  m_vao->Unbind();
}

void Curve::Draw(const Ref<system::Camera>& camara) {
  m_shader->Use();
  m_shader->SetUniform("uProjView", camara->GetProjectionMatrix() * camara->GetViewMatrix());
  m_vao->Bind();
  glLineWidth(2.0f);
  glDrawArrays(GL_LINES, 0, m_numPoints);
}

}  // namespace photon