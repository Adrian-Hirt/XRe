#pragma once

#include <xre/mesh.h>
#include <xre/model.h>
#include <tuple>

namespace ModelFactory {
// "Private" method using an anonymous namespace
namespace {
inline std::tuple<std::vector<Vertex>, std::vector<uint16_t>> getGroundVerticesAndIndices(float extent) {
  std::vector<Vertex> vertices = {{glm::vec3(-extent, 0.0f, -extent), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
                                  {glm::vec3(-extent, 0.0f, extent), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
                                  {glm::vec3(extent, 0.0f, extent), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
                                  {glm::vec3(extent, 0.0f, -extent), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)}};

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

inline Model createCube(glm::vec3 color, Material material) {
  auto [vertices, indices] = getCubeVerticesAndIndices();

  Mesh cube_mesh = Mesh(vertices, indices);
  return Model({cube_mesh}, color, material);
}

inline Model *createCubePtr(glm::vec3 color, Material material) {
  auto [vertices, indices] = getCubeVerticesAndIndices();

  Mesh cube_mesh = Mesh(vertices, indices);
  return new Model({cube_mesh}, color, material);
}

inline Model createGroundPlane(float extent, Material material) {
  auto [vertices, indices] = getGroundVerticesAndIndices(extent);

  Mesh ground_mesh = Mesh(vertices, indices);
  return Model({ground_mesh}, material);
}

inline Model createSphere(Material material) {
  Model sphere = Model(DATA_FOLDER "/models/sphere.obj", material);

  return sphere;
}
}; // namespace ModelFactory
