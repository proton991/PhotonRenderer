#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
namespace photon {
struct Vertex {
  using vec2 = glm::vec2;
  using vec3 = glm::vec3;

  Vertex() = default;

  Vertex(const vec3& position, const vec2& texture_coords)
      : position(position), uv(texture_coords) {}

  Vertex(const vec3& position, const vec2& texture_coords, const vec3& normal)
      : position(position), uv(texture_coords), normal(normal) {}

  //  Vertex(const vec3& position, const vec2& uv, const vec3& normal, const vec3& tangent)
  //      : position(position), uv(uv), normal(normal), tangent(tangent) {}

  vec3 position;
  vec2 uv;
  vec3 normal;
  //  vec3 tangent;
};
}  // namespace photon