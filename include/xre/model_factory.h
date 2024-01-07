#pragma once

#include <xre/mesh.h>
#include <xre/model.h>
#include <tuple>

namespace ModelFactory {
  inline Model createCube(DirectX::XMFLOAT4 color) {
    std::vector<vertex_t> cube_vertices {
      { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
      { DirectX::XMFLOAT3( 1.0f, -1.0f, 1.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
      { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f),  color, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
      { DirectX::XMFLOAT3( 1.0f, 1.0f, 1.0f),  color, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },

      { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
      { DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
      { DirectX::XMFLOAT3( 1.0f, -1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
      { DirectX::XMFLOAT3( 1.0f,  1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },

      { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) },
      { DirectX::XMFLOAT3(-1.0f, 1.0f,  1.0f), color, DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
      { DirectX::XMFLOAT3( 1.0f, 1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },
      { DirectX::XMFLOAT3( 1.0f, 1.0f,  1.0f), color, DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) },

      { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) },
      { DirectX::XMFLOAT3( 1.0f, -1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
      { DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f), color, DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },
      { DirectX::XMFLOAT3( 1.0f, -1.0f,  1.0f), color, DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) },

      { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), color, DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) },
      { DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f), color, DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
      { DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f), color, DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },
      { DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f), color, DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) },

      { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), color, DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) },
      { DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f), color, DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
      { DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f), color, DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },
      { DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f), color, DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) }
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
    return Model({ cube_mesh });
  }

  // "Private" method using an anonymous namespace
  namespace {
    inline std::tuple<std::vector<vertex_t>, std::vector<unsigned int>> getGroundVerticesAndIndices(float extent) {
      std::vector<vertex_t> vertices = {
        {  DirectX::XMFLOAT3(-extent, 0.0f, -extent), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),  DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(extent, 0.0f) },
        {  DirectX::XMFLOAT3(-extent, 0.0f,  extent), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),  DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
        {  DirectX::XMFLOAT3( extent, 0.0f,  extent), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),  DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, extent) },
        {  DirectX::XMFLOAT3( extent, 0.0f, -extent), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),  DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(extent, extent) }
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

  inline Model createHudQuad(float top_left_position_x, float top_left_position_y, float x_extend, float y_extend, DirectX::XMFLOAT4 color = { 0.2f, 0.2f, 0.2f, 1.0f }) {
    std::vector<vertex_t> vertices = {
      {  DirectX::XMFLOAT3(top_left_position_x,            top_left_position_y,            0.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
      {  DirectX::XMFLOAT3(top_left_position_x + x_extend, top_left_position_y,            0.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
      {  DirectX::XMFLOAT3(top_left_position_x + x_extend, top_left_position_y - y_extend, 0.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
      {  DirectX::XMFLOAT3(top_left_position_x,            top_left_position_y - y_extend, 0.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) }
    };

    std::vector<unsigned int> indices = {0, 1, 2, 3, 0, 2};

    Mesh quad_mesh = Mesh(vertices, indices);
    return Model({ quad_mesh });
  }
};
