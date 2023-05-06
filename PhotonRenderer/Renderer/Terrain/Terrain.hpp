#pragma once
#include <array>
#include <string>
#include <vector>
#include "Assets/Texture.hpp"
#include "Common/Array2D.hpp"
#include "Common/Base.hpp"
#include "GeoMipGrid.hpp"
// Reference:https://github.com/emeiri/ogldev/tree/master/Terrain7
namespace photon {
using asset::Texture2D;
namespace system {
class Camera;
}
namespace gl {
class ShaderProgram;
}

struct TerrainCreateInfo {
  int terrainSize;
  int patchSize;
  float roughness;
  float minHeight;
  float maxHeigt;
  float wordScale;
  float textureScale;
  std::vector<std::string> textureFiles;
};

class Terrain {
public:
  Terrain(const TerrainCreateInfo& info);
  ~Terrain();
  void Init();
  void Draw(const Ref<system::Camera>& camera, const glm::vec3& lightDir);
  float GetWorldScale() const { return m_worldScale; }
  float GetHeight(int x, int z) const { return m_heightMap.Get(x, z); }
  int GetSize() const { return m_terrainSize; }
  float GetTextureScale() const { return m_textureScale; }
  const glm::vec3& GetCenter() {
    auto center = m_geomipGrid.GetCenter();
    center.y    = center.y * 1.2f;
    return center;
  }

private:
  void Destroy();
  void CreateMidpointDisplacementF32(float roughness);
  void DiamondStep(int rectSize, float curHeight);
  void SquareStep(int rectSize, float curHeight);
  int m_terrainSize;
  int m_patchSize;
  float m_roughness;
  float m_minHeight;
  float m_maxHeigt;
  float m_worldScale;
  float m_textureScale;
  std::array<Ref<Texture2D>, 4> m_textures;
  Array2D<float> m_heightMap;
  GeomipGrid m_geomipGrid;
  Ref<gl::ShaderProgram> m_shader;
};

}  // namespace photon