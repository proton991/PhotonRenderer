#include "AssetLoader.hpp"
#include <stb_image.h>
#include <tiny_gltf.h>
#include <fstream>
#include <glm/glm.hpp>
#include "Assets/Model.hpp"
#include "Assets/Texture.hpp"
#include "Common/Logging.hpp"
#include "GltfUtils.hpp"
#include "Renderer/AABB.hpp"

namespace photon::util {

Ref<Model> AssetLoader::LoadModelOBJ(const std::string& path) {
  LOGI("Loading Model: {}", path);
  std::ifstream file(path);
  if (!file.is_open()) {
    LOGE("Failed to open {}", path);
    return nullptr;
  }
  std::vector<unsigned int> positionIndices, uvIndices, normalIndices;
  std::vector<glm::vec3> positions;
  std::vector<glm::vec2> uvs;
  std::vector<glm::vec3> normals;

  std::string currLine;
  while (std::getline(file, currLine)) {
    std::istringstream iss(currLine);
    std::string firstToken;
    iss >> firstToken;
    if (firstToken == "v") {
      glm::vec3 position;
      iss >> position.x >> position.y >> position.z;
      positions.push_back(position);
    } else if (firstToken == "vt") {
      glm::vec2 uv;
      iss >> uv.x >> uv.y;
      uvs.push_back(uv);
    } else if (firstToken == "vn") {
      glm::vec3 normal;
      iss >> normal.x >> normal.y >> normal.z;
      normals.push_back(normal);
    } else if (firstToken == "f") {
      char slash;
      uint32_t positionIndex, uvIndex, normalIndex;
      while (iss >> positionIndex >> slash >> uvIndex >> slash >> normalIndex) {
        positionIndices.push_back(positionIndex);
        uvIndices.push_back(uvIndex);
        normalIndices.push_back(normalIndex);
      }
    }
  }
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  // For each vertex of each triangle
  for (unsigned int i = 0; i < positionIndices.size(); i++) {

    // Get the indices of its attributes
    unsigned int vertexIndex = positionIndices[i];
    unsigned int uvIndex     = uvIndices[i];
    unsigned int normalIndex = normalIndices[i];

    // Get the attributes thanks to the index
    glm::vec3 position = positions[vertexIndex - 1];
    glm::vec2 uv       = uvs[uvIndex - 1];
    glm::vec3 normal   = normals[normalIndex - 1];

    indices.emplace_back(vertices.size());
    vertices.emplace_back(position, uv, normal);
  }
  return CreateRef<Model>(ExtractName(path), vertices, indices);
}

//Ref<Model> AssetLoader::LoadModelOBJ(const std::string& path) {
//  auto modelName = ExtractName(path);
//  LOGE("Loading Model: {}", modelName);
//  std::ifstream file(path);
//  if (!file.is_open()) {
//    if (file == NULL) {
//      LOGE("Failed to open {}", path);
//      return nullptr;
//    }
//  }
//  std::vector<unsigned int> positionIndices, uvIndices, normalIndices;
//  std::vector<glm::vec3> positions;
//  std::vector<glm::vec2> uvs;
//  std::vector<glm::vec3> normals;
//
//  FILE* file = fopen(path.c_str(), "r");
//  if (file == NULL) {
//    LOGE("Failed to open {}", path);
//    return nullptr;
//  }
//
//  while (1) {
//    char lineHeader[128];
//    // read the first word of the line
//    int res = fscanf(file, "%s", lineHeader);
//    if (res == EOF)
//      break;  // EOF = End Of File. Quit the loop.
//
//    if (strcmp(lineHeader, "v") == 0) {
//      glm::vec3 position;
//      fscanf(file, "%f %f %f\n", &position.x, &position.y, &position.z);
//      positions.push_back(position);
//    } else if (strcmp(lineHeader, "vt") == 0) {
//      glm::vec2 uv;
//      fscanf(file, "%f %f\n", &uv.x, &uv.y);
//      uvs.push_back(uv);
//    } else if (strcmp(lineHeader, "vn") == 0) {
//      glm::vec3 normal;
//      fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
//      normals.push_back(normal);
//    } else if (strcmp(lineHeader, "f") == 0) {
//      unsigned int positionIndex[3], uvIndex[3], normalIndex[3];
//      int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &positionIndex[0], &uvIndex[0],
//                           &normalIndex[0], &positionIndex[1], &uvIndex[1], &normalIndex[1],
//                           &positionIndex[2], &uvIndex[2], &normalIndex[2]);
//      if (matches != 9) {
//        LOGE("Failed to parse file");
//        fclose(file);
//        return nullptr;
//      }
//      positionIndices.push_back(positionIndex[0]);
//      positionIndices.push_back(positionIndex[1]);
//      positionIndices.push_back(positionIndex[2]);
//      uvIndices.push_back(uvIndex[0]);
//      uvIndices.push_back(uvIndex[1]);
//      uvIndices.push_back(uvIndex[2]);
//      normalIndices.push_back(normalIndex[0]);
//      normalIndices.push_back(normalIndex[1]);
//      normalIndices.push_back(normalIndex[2]);
//    } else {
//      // Probably a comment, eat up the rest of the line
//      char stupidBuffer[1000];
//      fgets(stupidBuffer, 1000, file);
//    }
//  }
//  std::vector<Vertex> vertices;
//  std::vector<uint32_t> indices;
//  // For each vertex of each triangle
//  for (unsigned int i = 0; i < positionIndices.size(); i++) {
//
//    // Get the indices of its attributes
//    unsigned int vertexIndex = positionIndices[i];
//    unsigned int uvIndex     = uvIndices[i];
//    unsigned int normalIndex = normalIndices[i];
//
//    // Get the attributes thanks to the index
//    glm::vec3 position = positions[vertexIndex - 1];
//    glm::vec2 uv       = uvs[uvIndex - 1];
//    glm::vec3 normal   = normals[normalIndex - 1];
//
//    indices.emplace_back(vertices.size());
//    vertices.emplace_back(position, uv, normal);
//  }
//  fclose(file);
//  return CreateRef<Model>(modelName, vertices, indices);
//}

Ref<Model> AssetLoader::LoadModelGLTF(const std::string& path) {
  LOGI("Loading Model: {}", path);
  tinygltf::Model gltf_model;
  tinygltf::TinyGLTF loader;
  std::string error;
  std::string warning;
  bool ret = loader.LoadASCIIFromFile(&gltf_model, &error, &warning, path.c_str());
  if (!warning.empty()) {
    LOGW(warning);
  }
  if (!error.empty()) {
    LOGE("gltf loader error: {}", error.c_str());
  }
  if (!ret) {
    LOGE("Failed to parse glTF");
    return nullptr;
  }
  const auto load_textures = [&](const tinygltf::Model& model) {
    tinygltf::Sampler defaultSampler;
    defaultSampler.minFilter = GL_LINEAR;
    defaultSampler.magFilter = GL_LINEAR;
    defaultSampler.wrapS     = GL_REPEAT;
    defaultSampler.wrapT     = GL_REPEAT;
    defaultSampler.wrapR     = GL_REPEAT;
    for (size_t i = 0; i < model.textures.size(); i++) {
      const auto& texture = model.textures[i];
      const auto& image   = model.images[texture.source];
      const auto& sampler = texture.sampler >= 0 ? model.samplers[texture.sampler] : defaultSampler;
      asset::TextureInfo info{};
      info.minFilter = sampler.minFilter != -1 ? sampler.minFilter : GL_LINEAR;
      info.magFilter = sampler.magFilter != -1 ? sampler.minFilter : GL_LINEAR;
      info.width     = image.width;
      info.height    = image.height;
      info.wrapR     = sampler.wrapR;
      info.wrapS     = sampler.wrapS;
      info.wrapT     = sampler.wrapT;

      if (sampler.minFilter == GL_NEAREST_MIPMAP_NEAREST ||
          sampler.minFilter == GL_NEAREST_MIPMAP_LINEAR ||
          sampler.minFilter == GL_LINEAR_MIPMAP_NEAREST ||
          sampler.minFilter == GL_LINEAR_MIPMAP_LINEAR) {
        info.generateMipmap = true;
      }
      m_textureCache.emplace_back(asset::Texture2D::Create(info, image.image.data()));
    }
  };
  static const std::unordered_map<std::string, int> c_AlphaModeValue = {
      {"OPAQUE", 0},
      {"BLEND", 1},
      {"MASK", 2},
  };

  const auto bind_material = [&](const auto materialIndex, asset::Mesh& mesh) {
    asset::PBRMaterial mesh_material{};
    if (materialIndex >= 0) {
      const tinygltf::Material& material = gltf_model.materials[materialIndex];
      mesh_material.name                 = material.name;
      mesh_material.alphaCutoff          = material.alphaCutoff;
      mesh_material.alphaMode            = c_AlphaModeValue.at(material.alphaMode);
      const auto& pbrMetallicRoughness   = material.pbrMetallicRoughness;
      mesh_material.baseColorFactor      = {(float)pbrMetallicRoughness.baseColorFactor[0],
                                            (float)pbrMetallicRoughness.baseColorFactor[1],
                                            (float)pbrMetallicRoughness.baseColorFactor[2],
                                            (float)pbrMetallicRoughness.baseColorFactor[3]};

      const auto baseColorTextureId = pbrMetallicRoughness.baseColorTexture.index;
      if (baseColorTextureId >= 0) {
        mesh_material.textures[asset::PBRComponent::BaseColor] = m_textureCache[baseColorTextureId];
      }

      const auto metallicRoughnessTextureId = pbrMetallicRoughness.metallicRoughnessTexture.index;
      if (metallicRoughnessTextureId >= 0) {
        mesh_material.textures[asset::PBRComponent::MetallicRoughness] =
            m_textureCache[metallicRoughnessTextureId];
      }
      mesh_material.metallicFactor  = pbrMetallicRoughness.metallicFactor;
      mesh_material.roughnessFactor = pbrMetallicRoughness.roughnessFactor;

      const auto normalTextureId = material.normalTexture.index;
      if (normalTextureId >= 0) {
        mesh_material.textures[asset::PBRComponent::Normal] = m_textureCache[normalTextureId];
      }

      const auto emissiveTextureId = material.emissiveTexture.index;
      if (emissiveTextureId >= 0) {
        mesh_material.textures[asset::PBRComponent::Emissive] = m_textureCache[emissiveTextureId];
      }
      mesh_material.emissiveFactor = {
          (float)material.emissiveFactor[0],
          (float)material.emissiveFactor[1],
          (float)material.emissiveFactor[2],
      };

      const auto occlusionTextureId = material.occlusionTexture.index;
      if (occlusionTextureId >= 0) {
        mesh_material.textures[asset::PBRComponent::Occlusion] = m_textureCache[occlusionTextureId];
      }
      mesh_material.occlusionStrength = material.occlusionTexture.strength;
    } else {
      // Apply default material
      // Defined here:
      // https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#reference-material
      // https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#reference-pbrmetallicroughness3
      LOGI("Using default white texture");
      mesh_material.textures[asset::PBRComponent::BaseColor] = Texture2D::CreateDefaultWhite();
    }
    mesh.material = std::move(mesh_material);
  };
  load_textures(gltf_model);
  std::unordered_map<int, glm::mat4> mesh_matrices;
  const std::function<void(int, const glm::mat4&)> extract_node_matrices =
      [&](int node_idx, const glm::mat4& parent_matrix) {
        const auto& node = gltf_model.nodes[node_idx];
        // get world matrix
        const glm::mat4 model_matrix = util::GetLocalToWorldMatrix(node, parent_matrix);
        if (node.mesh >= 0) {
          mesh_matrices[node.mesh] = model_matrix;
        }
        for (const auto child : node.children) {
          extract_node_matrices(child, model_matrix);
        }
      };
  if (gltf_model.defaultScene >= 0) {
    for (const auto node_idx : gltf_model.scenes[gltf_model.defaultScene].nodes) {
      extract_node_matrices(node_idx, glm::mat4(1.0f));
    }
  }
  const auto name = ExtractName(path);
  auto model      = asset::Model::Create(name);
  for (auto mesh_idx = 0; mesh_idx < gltf_model.meshes.size(); mesh_idx++) {
    auto& gl_mesh = gltf_model.meshes[mesh_idx];
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    for (auto primitive_index = 0; primitive_index < gl_mesh.primitives.size(); primitive_index++) {
      vertices.clear();
      indices.clear();
      auto& primitive = gl_mesh.primitives[primitive_index];
      util::ExtractGltfVertices(primitive, gltf_model, vertices);
      util::ExtractGltfIndices(primitive, gltf_model, indices);
      asset::Mesh mesh{vertices, indices};
      if (mesh_matrices.find(mesh_idx) != mesh_matrices.end()) {
        mesh.modelMatrix = mesh_matrices[mesh_idx];
      }
      bind_material(primitive.material, mesh);
      model->AttachMesh(mesh);
    }
  }

  glm::vec3 bbox_min, bbox_max;
  // compute boundary
  util::ComputeSceneBounds(gltf_model, bbox_min, bbox_max);
  AABB aabb{bbox_min, bbox_max};
  model->SetAABB(aabb);

  model->Translate(glm::vec3{0.0f, 0.0f, 0.0f});
  m_textureCache.clear();
  return model;
}

Ref<Texture2D> AssetLoader::LoadTexture(const std::string& path) {
  bool isHDR = ExtractExtension(path) == "hdr";
  int width, height, channels;
  stbi_set_flip_vertically_on_load(true);
  LOGT("Loading texture at path {}", path);
  auto* data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
  stbi_set_flip_vertically_on_load(false);

  if (!data) {
    LOGE("Failed to load texture {}", path);
    return nullptr;
  }
  asset::TextureInfo info{};
  info.width  = width;
  info.height = height;
  if (isHDR) {
    info.dataFormat     = GL_RGB;
    info.internalFormat = GL_RGB16F;
    info.dataType       = GL_FLOAT;
    info.wrapS          = GL_CLAMP_TO_EDGE;
    info.wrapT          = GL_CLAMP_TO_EDGE;
    info.minFilter      = GL_LINEAR;
    info.magFilter      = GL_LINEAR;
  }

  return Texture2D::Create(info, data);
}

Ref<Model> AssetLoader::LoadModel(const std::string& path) {
  auto modelFormat = ExtractExtension(path);
  if (modelFormat == "obj") {
    return LoadModelOBJ(path);
  } else if (modelFormat == "gltf") {
    return LoadModelGLTF(path);
  } else {
    LOGE("Model format {} not supported, sorry about that", modelFormat);
    return nullptr;
  }
}

std::string AssetLoader::ExtractName(const std::string& path) {
  size_t pos = path.find_last_of('/');
  // Extract the substring after the last '/'
  return path.substr(pos + 1);
}

std::string AssetLoader::ExtractExtension(const std::string& path) {
  size_t dotPos = path.find_last_of(".");
  // If there is no dot character or it's the last character in the path, return an empty string
  if (dotPos == std::string::npos || dotPos == path.length() - 1) {
    return "";
  }
  // Return the substring starting from the dot character to the end of the path
  return path.substr(dotPos + 1);
}
}  // namespace photon::util