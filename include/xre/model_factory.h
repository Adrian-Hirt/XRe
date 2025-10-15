#pragma once

#include <xre/mesh.h>
#include <xre/model.h>
#include <tuple>

namespace ModelFactory {
  // "Private" method using an anonymous namespace
  namespace {
    inline std::tuple<std::vector<Vertex>, std::vector<uint16_t>> getGroundVerticesAndIndices(float extent) {
      std::vector<Vertex> vertices = {
        {  glm::vec3(-extent, 0.0f, -extent), glm::vec3(1.0f, 0.0f, 0.0f) },
        {  glm::vec3(-extent, 0.0f,  extent), glm::vec3(0.0f, 1.0f, 0.0f) },
        {  glm::vec3( extent, 0.0f,  extent), glm::vec3(0.0f, 0.0f, 1.0f) },
        {  glm::vec3( extent, 0.0f, -extent), glm::vec3(1.0f, 1.0f, 1.0f) }
      };

      std::vector<uint16_t> indices = {
        0, 2, 1,
        0, 3, 2
      };

      return { vertices, indices };
    }

    inline std::tuple<std::vector<Vertex>, std::vector<uint16_t>> getCubeVerticesAndIndices() {
      std::vector<Vertex> vertices {
        { glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
        { glm::vec3( 1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
        { glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
        { glm::vec3( 1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f) },

        { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
        { glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
        { glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
        { glm::vec3( 1.0f,  1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f) },

        { glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
        { glm::vec3(-1.0f, 1.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
        { glm::vec3( 1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
        { glm::vec3( 1.0f, 1.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f) },

        { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
        { glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
        { glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
        { glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec3(0.0f, -1.0f, 0.0f) },

        { glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        { glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        { glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        { glm::vec3(1.0f,  1.0f,  1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },

        { glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f) },
        { glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(-1.0f, 0.0f, 0.0f) },
        { glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f) },
        { glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(-1.0f, 0.0f, 0.0f) }
      };

      std::vector<uint16_t> indices{
        2, 1, 0,    // side 1
        3, 1, 2,
        6, 5, 4,    // side 2
        7, 5, 6,
        10, 9, 8,    // side 3
        11, 9, 10,
        14, 13, 12,    // side 4
        15, 13, 14,
        18, 17, 16,    // side 5
        19, 17, 18,
        20, 22, 23,    // side 6
        23, 21, 20
      };

      return { vertices, indices };
    }
  }

  inline Model createCube(glm::vec3 color) {
    auto [vertices, indices] = getCubeVerticesAndIndices();

    Mesh cube_mesh = Mesh(vertices, indices);
    return Model({ cube_mesh }, color);
  }

  inline Model createGroundPlane(float extent) {
    auto [vertices, indices] = getGroundVerticesAndIndices(extent);

    Mesh ground_mesh = Mesh(vertices, indices);
    return Model({ ground_mesh });
  }

  inline Model createSphere() {
    Model sphere = Model(DATA_FOLDER "/models/sphere.obj");

    return sphere;
  }
};
