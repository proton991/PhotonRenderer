#pragma once

#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include "Renderer/Vertex.hpp"

namespace photon::util {

/**
 * Reference: https://github.com/vblanco20-1/vulkan-guide/blob/engine/asset-baker/asset_main.cpp
 */
void ExtractGltfIndices(tinygltf::Primitive& primitive, tinygltf::Model& model,
                        std::vector<uint32_t>& indices);
/**
 * Reference: https://github.com/vblanco20-1/vulkan-guide/blob/engine/asset-baker/asset_main.cpp
 */
void ExtractGltfVertices(tinygltf::Primitive& primitive, tinygltf::Model& model,
                         std::vector<Vertex>& vertices);
/**
 * Reference: https://github.com/vblanco20-1/vulkan-guide/blob/engine/asset-baker/asset_main.cpp
 */
void UnpackGltfBuffer(tinygltf::Model& model, const tinygltf::Accessor& accessor,
                      std::vector<uint8_t>& outBuffer);

/**
 * Reference: https://gitlab.com/gltf-viewer-tutorial/gltf-viewer/-/blob/tutorial-v1/src/utils/gltf.cpp
 */
glm::mat4 GetLocalToWorldMatrix(const tinygltf::Node& node, const glm::mat4& parentMatrix);

/**
 * Reference: https://gitlab.com/gltf-viewer-tutorial/gltf-viewer/-/blob/tutorial-v1/src/utils/gltf.cpp
 */
void ComputeSceneBounds(const tinygltf::Model& model, glm::vec3& bboxMin, glm::vec3& bboxMax);
}  // namespace photon::util
