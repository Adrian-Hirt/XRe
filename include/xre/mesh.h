#pragma once

// DirectX includes
#include <d3d11.h>

// Include vector header
#include <vector>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/shader.h>

class Mesh {
public:
  Mesh();
  Mesh(ID3D11Device *device, ID3D11DeviceContext *device_context, std::vector<vertex> vertices, std::vector<unsigned int> indices);

  void render();
  void render(Shader shader);

private:
  ID3D11Device *device;
  ID3D11DeviceContext *device_context;

  ID3D11Buffer *vertex_buffer; // Vertex buffer
  ID3D11Buffer *index_buffer;  // Index buffer

  size_t vertex_count;
  size_t index_count;
};
