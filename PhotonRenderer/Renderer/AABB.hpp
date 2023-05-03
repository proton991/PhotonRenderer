#pragma once
#include <glm/glm.hpp>
#include "Common/Base.hpp"

namespace photon {
struct Line;
struct AABB {
  AABB() = default;
  AABB(const glm::vec3& _min, const glm::vec3& _max);

  glm::vec3 GetCenter() const;

  // translate the center of the bbx to target_pos
  void Translate(const glm::vec3& pos);
  void Scale(float factor);

  void FillLinesData(Ref<Line>& data) const;

  glm::vec3 posMin;
  glm::vec3 posMax;
  glm::vec3 diag;
};
}  // namespace photon
