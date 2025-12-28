#pragma once

// XRe includes
#include <xre/mesh.h>
#include <xre/model.h>
#include <xre/vulkan_handler.h>

// Other includes
#include <tuple>
#include <memory>

namespace ModelFactory {
// "Private" method using an anonymous namespace
namespace {
inline std::tuple<std::vector<Vertex>, std::vector<uint16_t>> getPlaneVerticesAndIndices(float extent) {
  std::vector<Vertex> vertices = {{glm::vec3(-extent, 0.0f, -extent), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
                                  {glm::vec3(-extent, 0.0f, extent), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
                                  {glm::vec3(extent, 0.0f, extent), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
                                  {glm::vec3(extent, 0.0f, -extent), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)}};

  std::vector<uint16_t> indices = {0, 2, 1, 0, 3, 2};

  return {vertices, indices};
}

inline std::tuple<std::vector<Vertex>, std::vector<uint16_t>> getCubeVerticesAndIndices() {
  std::vector<Vertex> vertices{
      {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},   {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
      {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},    {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},

      {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f)}, {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
      {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},  {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},

      {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},   {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
      {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},    {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},

      {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)}, {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
      {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},  {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},

      {glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},   {glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
      {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},    {glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},

      {glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)}, {glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
      {glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},  {glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)}};

  std::vector<uint16_t> indices{
      2,  1,  0,  3,  1,  2,  // side 1
      6,  5,  4,  7,  5,  6,  // side 2
      10, 9,  8,  11, 9,  10, // side 3
      14, 13, 12, 15, 13, 14, // side 4
      18, 17, 16, 19, 17, 18, // side 5
      20, 22, 23, 23, 21, 20  // side 6
  };

  return {vertices, indices};
}
} // namespace

inline std::shared_ptr<Model> createCube(glm::vec3 color, std::shared_ptr<Material> material, std::shared_ptr<VulkanHandler> vulkan_handler) {
  auto [vertices, indices] = getCubeVerticesAndIndices();

  Mesh cube_mesh = Mesh(vertices, indices, vulkan_handler);
  return std::make_shared<Model>(std::vector<Mesh>{cube_mesh}, color, material);
}

inline std::shared_ptr<Model> createPlane(float extent, std::shared_ptr<Material> material, std::shared_ptr<VulkanHandler> vulkan_handler) {
  auto [vertices, indices] = getPlaneVerticesAndIndices(extent);

  Mesh plane_mesh = Mesh(vertices, indices, vulkan_handler);
  return std::make_shared<Model>(std::vector<Mesh>{plane_mesh}, material);
}

inline std::shared_ptr<Model> createSphere(std::shared_ptr<Material> material, std::shared_ptr<VulkanHandler> vulkan_handler) {
  return std::make_shared<Model>(DATA_FOLDER "/models/sphere.obj", material, vulkan_handler);
}
}; // namespace ModelFactory
