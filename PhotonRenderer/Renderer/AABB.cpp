#include "AABB.hpp"
#include "Graphics/MeshBuffer.hpp"
#include "Line.hpp"

namespace photon {
using namespace photon::gl;
AABB::AABB(const glm::vec3& _min, const glm::vec3& _max) : posMin(_min), posMax(_max) {
  diag = posMax - posMin;
}

void AABB::Scale(float factor) {
  auto i  = glm::vec3(1.0f, 0.0f, 0.0f);
  auto j  = glm::vec3(0.0f, 1.0f, 0.0f);
  auto k  = glm::vec3(0.0f, 0.0f, 1.0f);
  float x = glm::dot(diag, i);
  float y = glm::dot(diag, j);
  float z = glm::dot(diag, k);
  x *= factor;
  y *= factor;
  z *= factor;
  posMin = glm::vec3(-x * 0.5, -y * 0.5, -z * 0.5);
  posMax = glm::vec3(x * 0.5, -y * 0.5, z * 0.5);
  diag   = posMax - posMin;
}

void AABB::Translate(const glm::vec3& pos) {
  glm::vec3 delta = pos - GetCenter();
  posMin += delta;
  posMax += delta;
  diag = posMax - posMin;
}

glm::vec3 AABB::GetCenter() const {
  return (posMin + posMax) * 0.5f;
}

void AABB::FillLinesData(Ref<Line>& data) const {
  glm::vec3 i = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 j = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 k = glm::vec3(0.0f, 0.0f, 1.0f);

  float x = glm::dot(diag, i);
  float y = glm::dot(diag, j);
  float z = glm::dot(diag, k);
  auto yellow{glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)};

  auto a = glm::vec3(posMin.x, posMin.y, posMin.z + z);
  auto c = glm::vec3(posMin.x + x, posMin.y, posMin.z);
  auto b = glm::vec3(posMin.x + x, posMin.y, posMin.z + z);
  auto e = glm::vec3(posMin.x, posMin.y + y, posMin.z);
  auto d = glm::vec3(posMin.x + x, posMin.y + y, posMin.z);
  auto f = glm::vec3(posMin.x, posMin.y + y, posMin.z + z);

  data->lineVertices = {
      {posMin, yellow}, {a, yellow}, {posMin, yellow}, {c, yellow}, {posMin, yellow}, {e, yellow},

      {e, yellow},      {f, yellow}, {e, yellow},      {d, yellow},

      {posMax, yellow}, {b, yellow}, {posMax, yellow}, {d, yellow}, {posMax, yellow}, {f, yellow},

      {b, yellow},      {a, yellow}, {b, yellow},      {c, yellow},

      {a, yellow},      {f, yellow},

      {d, yellow},      {c, yellow},
  };
  auto vbo = VertexBuffer::Create(data->lineVertices.size() * sizeof(LineVertex),
                                  data->lineVertices.data());
  vbo->SetBufferView({
      {"aPosition", BufferDataType::Vec3f},
      {"aColor", BufferDataType::Vec4f},
  });
  data->vao = VertexArray::Create();
  data->vao->Bind();
  data->vao->AttachVertexBuffer(vbo);
}
}  // namespace photon