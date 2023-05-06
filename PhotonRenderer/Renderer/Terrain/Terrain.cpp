#include "Terrain.hpp"
#include "Common/Logging.hpp"
#include "Common/Math.hpp"
#include "Graphics/Shader.hpp"
#include "Systems/CameraSystem.hpp"

namespace photon {
Terrain::Terrain(const TerrainCreateInfo& info) {
  m_worldScale   = info.worldScale;
  m_textureScale = info.textureScale;
  m_terrainSize  = info.terrainSize;
  m_patchSize    = info.patchSize;
  m_minHeight    = info.minHeight;
  m_maxHeigt     = info.maxHeigt;
  m_roughness    = info.roughness;

  if (info.textureFiles.size() != m_textures.size()) {
    LOGE("Number of terrain textures is not equal to {}", m_textures.size());
    exit(0);
  }
  for (auto i = 0u; i < m_textures.size(); i++) {
    m_textures[i] = CreateRef<Texture2D>(info.textureFiles[i]);
  }
  // init shader program
  gl::ShaderProgramCreateInfo shaderInfo = {
      "TerrainShader",
      {{"Data/Shaders/terrain.vs.glsl", "vertex"}, {"Data/Shaders/terrain.fs.glsl", "fragment"}}};
  m_shader = gl::ShaderProgramFactory::CreateShaderProgram(shaderInfo);
}

Terrain::~Terrain() {
  Destroy();
}

void Terrain::Destroy() {
  m_heightMap.Destroy();
  m_geomipGrid.Destroy();
}

void Terrain::CreateMidpointDisplacementF32(float roughness) {
  int rectSize       = math::NextPowerOfTwo(m_terrainSize);
  float curHeight    = (float)rectSize / 2.0f;
  float heightReduce = pow(2.0f, -roughness);

  while (rectSize > 0) {

    DiamondStep(rectSize, curHeight);

    SquareStep(rectSize, curHeight);

    rectSize /= 2;
    curHeight *= heightReduce;
  }
}

void Terrain::DiamondStep(int rectSize, float curHeight) {
  int HalfRectSize = rectSize / 2;

  for (int y = 0; y < m_terrainSize; y += rectSize) {
    for (int x = 0; x < m_terrainSize; x += rectSize) {
      int nextX = (x + rectSize) % m_terrainSize;
      int nextY = (y + rectSize) % m_terrainSize;

      if (nextX < x) {
        nextX = m_terrainSize - 1;
      }

      if (nextY < y) {
        nextY = m_terrainSize - 1;
      }

      float TopLeft     = m_heightMap.Get(x, y);
      float TopRight    = m_heightMap.Get(nextX, y);
      float BottomLeft  = m_heightMap.Get(x, nextY);
      float BottomRight = m_heightMap.Get(nextX, nextY);

      int mid_x = (x + HalfRectSize) % m_terrainSize;
      int mid_y = (y + HalfRectSize) % m_terrainSize;

      float RandValue = math::RandomFloat(-curHeight, curHeight);
      float MidPoint  = (TopLeft + TopRight + BottomLeft + BottomRight) / 4.0f;

      m_heightMap.Set(mid_x, mid_y, MidPoint + RandValue);
    }
  }
}

void Terrain::SquareStep(int RectSize, float CurHeight) {
  int HalfRectSize = RectSize / 2;

  for (int y = 0; y < m_terrainSize; y += RectSize) {
    for (int x = 0; x < m_terrainSize; x += RectSize) {
      int next_x = (x + RectSize) % m_terrainSize;
      int next_y = (y + RectSize) % m_terrainSize;

      if (next_x < x) {
        next_x = m_terrainSize - 1;
      }

      if (next_y < y) {
        next_y = m_terrainSize - 1;
      }

      int mid_x = (x + HalfRectSize) % m_terrainSize;
      int mid_y = (y + HalfRectSize) % m_terrainSize;

      int prev_mid_x = (x - HalfRectSize + m_terrainSize) % m_terrainSize;
      int prev_mid_y = (y - HalfRectSize + m_terrainSize) % m_terrainSize;

      float CurTopLeft  = m_heightMap.Get(x, y);
      float CurTopRight = m_heightMap.Get(next_x, y);
      float CurCenter   = m_heightMap.Get(mid_x, mid_y);
      float PrevYCenter = m_heightMap.Get(mid_x, prev_mid_y);
      float CurBotLeft  = m_heightMap.Get(x, next_y);
      float PrevXCenter = m_heightMap.Get(prev_mid_x, mid_y);

      float CurLeftMid = (CurTopLeft + CurCenter + CurBotLeft + PrevXCenter) / 4.0f +
                         math::RandomFloat(-CurHeight, CurHeight);
      float CurTopMid = (CurTopLeft + CurCenter + CurTopRight + PrevYCenter) / 4.0f +
                        math::RandomFloat(-CurHeight, CurHeight);

      m_heightMap.Set(mid_x, y, CurTopMid);
      m_heightMap.Set(x, mid_y, CurLeftMid);
    }
  }
}

void Terrain::Init() {
  m_heightMap.InitArray2D(m_terrainSize, m_terrainSize, 0.0f);
  CreateMidpointDisplacementF32(m_roughness);
  m_heightMap.Normalize(m_minHeight, m_maxHeigt);
  m_geomipGrid.CreateGeomipGrid(m_terrainSize, m_terrainSize, m_patchSize, this);
}

void Terrain::Draw(const Ref<system::Camera>& camera, const glm::vec3& lightDir) {
  auto VP = camera->GetProjectionMatrix() * camera->GetViewMatrix();

  m_shader->Use();
  m_shader->SetUniform("gVP", VP);

  for (int i = 0; i < m_textures.size(); i++) {
    if (m_textures[i]) {
      m_textures[i]->Bind(i);
    }
  }
  double range = m_maxHeigt - m_minHeight;
  double step  = range / 4.0;

  m_shader->SetUniform("gReversedLightDir", {1.0, 1.0f, 1.0f});
  m_shader->SetUniform("gMinHeight", m_minHeight);
  m_shader->SetUniform("gMaxHeight", m_maxHeigt);
  m_geomipGrid.Render(camera->GetPos());
}

}  // namespace photon