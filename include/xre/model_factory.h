#pragma once

#include <xre/mesh.h>
#include <xre/model.h>
#include <tuple>

namespace ModelFactory {
  inline Model createCube(DirectX::XMFLOAT4 color) {
    std::vector<vertex_t> cube_vertices {
      { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
      { DirectX::XMFLOAT3( 1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
      { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
      { DirectX::XMFLOAT3( 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },

      { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
      { DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
      { DirectX::XMFLOAT3( 1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
      { DirectX::XMFLOAT3( 1.0f,  1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },

      { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) },
      { DirectX::XMFLOAT3(-1.0f, 1.0f,  1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
      { DirectX::XMFLOAT3( 1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },
      { DirectX::XMFLOAT3( 1.0f, 1.0f,  1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) },

      { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) },
      { DirectX::XMFLOAT3( 1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
      { DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },
      { DirectX::XMFLOAT3( 1.0f, -1.0f,  1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) },

      { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) },
      { DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
      { DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },
      { DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) },

      { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) },
      { DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
      { DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },
      { DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) }
    };

    std::vector<unsigned int> cube_indices{
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

    Mesh cube_mesh = Mesh(cube_vertices, cube_indices);
    return Model({ cube_mesh }, color);
  }

  // "Private" method using an anonymous namespace
  namespace {
    inline std::tuple<std::vector<vertex_t>, std::vector<unsigned int>> getGroundVerticesAndIndices(float extent) {
      std::vector<vertex_t> vertices = {
        {  DirectX::XMFLOAT3(-extent, 0.0f, -extent), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(extent, 0.0f) },
        {  DirectX::XMFLOAT3(-extent, 0.0f,  extent), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
        {  DirectX::XMFLOAT3( extent, 0.0f,  extent), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, extent) },
        {  DirectX::XMFLOAT3( extent, 0.0f, -extent), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(extent, extent) }
      };

      std::vector<unsigned int> indices = {0, 2, 1, 0, 3, 2};

      return { vertices, indices };
    }
  }

  inline Model createGroundPlane(float extent) {
    auto [vertices, indices] = getGroundVerticesAndIndices(extent);

    Mesh ground_mesh = Mesh(vertices, indices);
    return Model({ ground_mesh });
  }

  inline Model createGroundPlane(float extent, const char *texture_path) {
    auto [vertices, indices] = getGroundVerticesAndIndices(extent);

    Mesh ground_mesh = Mesh(vertices, indices, texture_path);
    return Model({ ground_mesh });
  }

  inline Model createSphere(float radius) {
    Model sphere = Model(DATA_FOLDER "/models/sphere.obj");

    sphere.scale(radius, radius, radius);

    return sphere;
  }
};
