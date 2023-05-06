#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "LodManager.hpp"
// Reference:https://github.com/emeiri/ogldev/tree/master/Terrain7
namespace photon {
class Terrain;
class GeomipGrid {
public:
  GeomipGrid();

  ~GeomipGrid();

  void CreateGeomipGrid(int Width, int Depth, int PatchSize, const Terrain* pTerrain);

  void Destroy();

  void Render(const glm::vec3& CameraPos);

  const glm::vec3& GetCenter() { return m_center; }

private:
  struct Vertex {
    glm::vec3 Pos;
    glm::vec2 Tex;
    glm::vec3 Normal{0.0f, 0.0f, 0.0f};

    void InitVertex(const Terrain* pTerrain, int x, int z);
  };

  glm::vec3 m_center;

  void CreateGLState();

  void PopulateBuffers(const Terrain* pTerrain);

  void InitVertices(const Terrain* pTerrain, std::vector<Vertex>& Vertices);

  int InitIndices(std::vector<uint32_t>& Indices);

  int InitIndicesLOD(int Index, std::vector<uint32_t>& Indices, int lod);

  int InitIndicesLODSingle(int Index, std::vector<uint32_t>& Indices, int lodCore, int lodLeft,
                           int lodRight, int lodTop, int lodBottom);

  void CalcNormals(std::vector<Vertex>& Vertices, std::vector<uint32_t>& Indices);

  uint32_t AddTriangle(uint32_t Index, std::vector<uint32_t>& Indices, uint32_t v1, uint32_t v2,
                       uint32_t v3);

  uint32_t CreateTriangleFan(int Index, std::vector<uint32_t>& Indices, int lodCore, int lodLeft,
                             int lodRight, int lodTop, int lodBottom, int x, int z);

  int CalcNumIndices();

  int m_width     = 0;
  int m_depth     = 0;
  int m_patchSize = 0;
  int m_maxLOD    = 0;
  GLuint m_vao    = 0;
  GLuint m_vb     = 0;
  GLuint m_ib     = 0;

  struct SingleLodInfo {
    int Start = 0;
    int Count = 0;
  };

#define LEFT 2
#define RIGHT 2
#define TOP 2
#define BOTTOM 2

  struct LodInfo {
    SingleLodInfo info[LEFT][RIGHT][TOP][BOTTOM];
  };

  std::vector<LodInfo> m_lodInfo;
  int m_numPatchesX = 0;
  int m_numPatchesZ = 0;
  LodManager m_lodManager;
};

}  // namespace photon