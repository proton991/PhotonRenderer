#include "GeoMipGrid.hpp"
#include "Common/Math.hpp"
#include "Terrain.hpp"

int gShowPoints = 0;
namespace photon {
GeomipGrid::GeomipGrid() {}

GeomipGrid::~GeomipGrid() {
  Destroy();
}

void GeomipGrid::Destroy() {
  if (m_vao > 0) {
    glDeleteVertexArrays(1, &m_vao);
  }

  if (m_vb > 0) {
    glDeleteBuffers(1, &m_vb);
  }

  if (m_ib > 0) {
    glDeleteBuffers(1, &m_ib);
  }
}

void GeomipGrid::CreateGeomipGrid(int Width, int Depth, int PatchSize, const Terrain* pTerrain) {
  if ((Width - 1) % (PatchSize - 1) != 0) {
    int RecommendedWidth = ((Width - 1 + PatchSize - 1) / (PatchSize - 1)) * (PatchSize - 1) + 1;
    printf("Width minus 1 (%d) must be divisible by PatchSize minus 1 (%d)\n", Width, PatchSize);
    printf("Try using Width = %d\n", RecommendedWidth);
    exit(0);
  }

  if ((Depth - 1) % (PatchSize - 1) != 0) {
    int RecommendedDepth = ((Depth - 1 + PatchSize - 1) / (PatchSize - 1)) * (PatchSize - 1) + 1;
    printf("Depth minus 1 (%d) must be divisible by PatchSize minus 1 (%d)\n", Depth, PatchSize);
    printf("Try using Width = %d\n", RecommendedDepth);
    exit(0);
  }

  if (PatchSize < 3) {
    printf("The minimum patch size is 3 (%d)\n", PatchSize);
    exit(0);
  }

  if (PatchSize % 2 == 0) {
    printf("Patch size must be an odd number (%d)\n", PatchSize);
    exit(0);
  }

  m_width     = Width;
  m_depth     = Depth;
  m_patchSize = PatchSize;

  m_numPatchesX = (Width - 1) / (PatchSize - 1);
  m_numPatchesZ = (Depth - 1) / (PatchSize - 1);

  float WorldScale = pTerrain->GetWorldScale();
  m_maxLOD = m_lodManager.InitLodManager(PatchSize, m_numPatchesX, m_numPatchesZ, WorldScale);
  m_lodInfo.resize(m_maxLOD + 1);

  CreateGLState();

  PopulateBuffers(pTerrain);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GeomipGrid::CreateGLState() {
  glGenVertexArrays(1, &m_vao);

  glBindVertexArray(m_vao);

  glGenBuffers(1, &m_vb);

  glBindBuffer(GL_ARRAY_BUFFER, m_vb);

  glGenBuffers(1, &m_ib);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);

  int POS_LOC    = 0;
  int TEX_LOC    = 1;
  int NORMAL_LOC = 2;

  size_t NumFloats = 0;

  glEnableVertexAttribArray(POS_LOC);
  glVertexAttribPointer(POS_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (const void*)(NumFloats * sizeof(float)));
  NumFloats += 3;

  glEnableVertexAttribArray(TEX_LOC);
  glVertexAttribPointer(TEX_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (const void*)(NumFloats * sizeof(float)));
  NumFloats += 2;

  glEnableVertexAttribArray(NORMAL_LOC);
  glVertexAttribPointer(NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (const void*)(NumFloats * sizeof(float)));
  NumFloats += 3;
}

void GeomipGrid::PopulateBuffers(const Terrain* pTerrain) {
  std::vector<Vertex> Vertices;
  Vertices.resize(m_width * m_depth);
  printf("Preparing space for %zu vertices\n", Vertices.size());
  InitVertices(pTerrain, Vertices);

  int NumIndices = CalcNumIndices();
  std::vector<unsigned int> Indices;
  Indices.resize(NumIndices);

  NumIndices = InitIndices(Indices);
  printf("Final number of indices %d\n", NumIndices);

  CalcNormals(Vertices, Indices);

  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * Vertices.size(), &Vertices[0],
               GL_STATIC_DRAW);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * NumIndices, &Indices[0],
               GL_STATIC_DRAW);
}

int GeomipGrid::CalcNumIndices() {
  int NumQuads                = (m_patchSize - 1) * (m_patchSize - 1);
  int NumIndices              = 0;
  int MaxPermutationsPerLevel = 16;  // true/false for each of the four sides
  const int IndicesPerQuad    = 6;   // two triangles
  for (int lod = 0; lod <= m_maxLOD; lod++) {
    printf("LOD %d: num quads %d\n", lod, NumQuads);
    NumIndices += NumQuads * IndicesPerQuad * MaxPermutationsPerLevel;
    NumQuads /= 4;
  }
  printf("Initial number of indices %d\n", NumIndices);
  return NumIndices;
}

void GeomipGrid::Vertex::InitVertex(const Terrain* pTerrain, int x, int z) {
  float y = pTerrain->GetHeight(x, z);

  float WorldScale = pTerrain->GetWorldScale();
  Pos              = glm::vec3(x * WorldScale, y, z * WorldScale);

  float Size         = (float)pTerrain->GetSize();
  float TextureScale = pTerrain->GetTextureScale();
  Tex                = glm::vec2(TextureScale * (float)x / Size, TextureScale * (float)z / Size);
}

void GeomipGrid::InitVertices(const Terrain* pTerrain, std::vector<Vertex>& Vertices) {
  int Index = 0;

  for (int z = 0; z < m_depth; z++) {
    for (int x = 0; x < m_width; x++) {
      assert(Index < Vertices.size());
      Vertices[Index].InitVertex(pTerrain, x, z);
      Vertices[Index].Pos.x -= (m_width - 1);
      Vertices[Index].Pos.z -= (m_depth - 1);
      Index++;
    }
  }
  m_center = Vertices[Index / 2].Pos;
  assert(Index == Vertices.size());
}

int GeomipGrid::InitIndices(std::vector<unsigned int>& Indices) {
  int Index = 0;

  for (int lod = 0; lod <= m_maxLOD; lod++) {
    printf("*** Init indices lod %d ***\n", lod);
    Index = InitIndicesLOD(Index, Indices, lod);
    printf("\n");
  }

  return Index;
}

int GeomipGrid::InitIndicesLOD(int Index, std::vector<unsigned int>& Indices, int lod) {
  int TotalIndicesForLOD = 0;

  for (int l = 0; l < LEFT; l++) {
    for (int r = 0; r < RIGHT; r++) {
      for (int t = 0; t < TOP; t++) {
        for (int b = 0; b < BOTTOM; b++) {
          m_lodInfo[lod].info[l][r][t][b].Start = Index;
          Index = InitIndicesLODSingle(Index, Indices, lod, lod + l, lod + r, lod + t, lod + b);

          m_lodInfo[lod].info[l][r][t][b].Count = Index - m_lodInfo[lod].info[l][r][t][b].Start;
          TotalIndicesForLOD += m_lodInfo[lod].info[l][r][t][b].Count;
        }
      }
    }
  }

  printf("Total indices for LOD: %d\n", TotalIndicesForLOD);

  return Index;
}

int GeomipGrid::InitIndicesLODSingle(int Index, std::vector<unsigned int>& Indices, int lodCore,
                                     int lodLeft, int lodRight, int lodTop, int lodBottom) {
  int FanStep = POWI(2, lodCore + 1);  // lod = 0 --> 2, lod = 1 --> 4, lod = 2 --> 8, etc
  int EndPos =
      m_patchSize - 1 -
      FanStep;  // patch size 5, fan step 2 --> EndPos = 2; patch size 9, fan step 2 --> EndPos = 6

  for (int z = 0; z <= EndPos; z += FanStep) {
    for (int x = 0; x <= EndPos; x += FanStep) {
      int lLeft   = x == 0 ? lodLeft : lodCore;
      int lRight  = x == EndPos ? lodRight : lodCore;
      int lBottom = z == 0 ? lodBottom : lodCore;
      int lTop    = z == EndPos ? lodTop : lodCore;

      Index = CreateTriangleFan(Index, Indices, lodCore, lLeft, lRight, lTop, lBottom, x, z);
    }
  }

  return Index;
}

uint32_t GeomipGrid::CreateTriangleFan(int Index, std::vector<unsigned int>& Indices, int lodCore,
                                       int lodLeft, int lodRight, int lodTop, int lodBottom, int x,
                                       int z) {
  int StepLeft   = POWI(2, lodLeft);  // because LOD starts at zero...
  int StepRight  = POWI(2, lodRight);
  int StepTop    = POWI(2, lodTop);
  int StepBottom = POWI(2, lodBottom);
  int StepCenter = POWI(2, lodCore);

  uint32_t IndexCenter = (z + StepCenter) * m_width + x + StepCenter;

  // first up
  uint32_t IndexTemp1 = z * m_width + x;
  uint32_t IndexTemp2 = (z + StepLeft) * m_width + x;

  Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

  // second up
  if (lodLeft == lodCore) {
    IndexTemp1 = IndexTemp2;
    IndexTemp2 += StepLeft * m_width;

    Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);
  }

  // first right
  IndexTemp1 = IndexTemp2;
  IndexTemp2 += StepTop;

  Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

  // second right
  if (lodTop == lodCore) {
    IndexTemp1 = IndexTemp2;
    IndexTemp2 += StepTop;

    Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);
  }

  // first down
  IndexTemp1 = IndexTemp2;
  IndexTemp2 -= StepRight * m_width;

  Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

  // second down
  if (lodRight == lodCore) {
    IndexTemp1 = IndexTemp2;
    IndexTemp2 -= StepRight * m_width;

    Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);
  }

  // first left
  IndexTemp1 = IndexTemp2;
  IndexTemp2 -= StepBottom;

  Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);

  // second left
  if (lodBottom == lodCore) {
    IndexTemp1 = IndexTemp2;
    IndexTemp2 -= StepBottom;

    Index = AddTriangle(Index, Indices, IndexCenter, IndexTemp1, IndexTemp2);
  }

  return Index;
}

uint32_t GeomipGrid::AddTriangle(uint32_t Index, std::vector<uint32_t>& Indices, uint32_t v1,
                                 uint32_t v2, uint32_t v3) {
  assert(Index < Indices.size());
  Indices[Index++] = v1;
  assert(Index < Indices.size());
  Indices[Index++] = v2;
  assert(Index < Indices.size());
  Indices[Index++] = v3;

  return Index;
}

void GeomipGrid::CalcNormals(std::vector<Vertex>& Vertices, std::vector<uint32_t>& Indices) {
  unsigned int Index = 0;

  // Accumulate each triangle normal into each of the triangle vertices
  for (int z = 0; z < m_depth - 1; z += (m_patchSize - 1)) {
    for (int x = 0; x < m_width - 1; x += (m_patchSize - 1)) {
      int BaseVertex = z * m_width + x;
      //printf("Base index %d\n", BaseVertex);
      int NumIndices = m_lodInfo[0].info[0][0][0][0].Count;
      for (int i = 0; i < NumIndices; i += 3) {
        unsigned int Index0 = BaseVertex + Indices[i];
        unsigned int Index1 = BaseVertex + Indices[i + 1];
        unsigned int Index2 = BaseVertex + Indices[i + 2];
        glm::vec3 v1        = Vertices[Index1].Pos - Vertices[Index0].Pos;
        glm::vec3 v2        = Vertices[Index2].Pos - Vertices[Index0].Pos;
        glm::vec3 Normal    = glm::cross(v1, v2);
        Normal              = glm::normalize(Normal);

        Vertices[Index0].Normal += Normal;
        Vertices[Index1].Normal += Normal;
        Vertices[Index2].Normal += Normal;
      }
    }
  }

  // Normalize all the vertex normals
  for (unsigned int i = 0; i < Vertices.size(); i++) {
    Vertices[i].Normal = glm::normalize(Vertices[i].Normal);
  }
}

void GeomipGrid::Render(const glm::vec3& CameraPos) {
  m_lodManager.Update(CameraPos);

  glBindVertexArray(m_vao);

  if (gShowPoints > 0) {
    glDrawElementsBaseVertex(GL_POINTS, m_lodInfo[0].info[0][0][0][0].Count, GL_UNSIGNED_INT,
                             (void*)0, 0);
  }

  if (gShowPoints != 2) {
    for (int PatchZ = 0; PatchZ < m_numPatchesZ; PatchZ++) {
      for (int PatchX = 0; PatchX < m_numPatchesX; PatchX++) {
        const LodManager::PatchLod& plod = m_lodManager.GetPatchLod(PatchX, PatchZ);
        int C                            = plod.Core;
        int L                            = plod.Left;
        int R                            = plod.Right;
        int T                            = plod.Top;
        int B                            = plod.Bottom;

        size_t BaseIndex = sizeof(unsigned int) * m_lodInfo[C].info[L][R][T][B].Start;

        int z          = PatchZ * (m_patchSize - 1);
        int x          = PatchX * (m_patchSize - 1);
        int BaseVertex = z * m_width + x;
        //            printf("%d\n", BaseVertex);

        glDrawElementsBaseVertex(GL_TRIANGLES, m_lodInfo[C].info[L][R][T][B].Count, GL_UNSIGNED_INT,
                                 (void*)BaseIndex, BaseVertex);
      }
    }
  }

  glBindVertexArray(0);
}
}  // namespace photon