#pragma once

// DirectX includes
#include <d3d11.h>
#include <DirectXTex/WICTextureLoader11.h>

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
  Mesh(ID3D11Device *device, ID3D11DeviceContext *device_context, std::vector<vertex> vertices, std::vector<unsigned int> indices, const char *texture_path);

  void render();
  void render(Shader shader);

private:
  ID3D11Device *device;
  ID3D11DeviceContext *device_context;

  ID3D11Buffer *vertex_buffer; // Vertex buffer
  ID3D11Buffer *index_buffer;  // Index buffer

  ID3D11Resource *p_texture = NULL;
  ID3D11ShaderResourceView *p_texture_view = NULL;

  size_t vertex_count;
  size_t index_count;

  void initialize(ID3D11Device *device, ID3D11DeviceContext *device_context, std::vector<vertex> vertices, std::vector<unsigned int> indices);
};
