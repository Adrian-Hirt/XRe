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
  Mesh(std::vector<vertex_t> vertices, std::vector<unsigned int> indices);
  Mesh(std::vector<vertex_t> vertices, std::vector<unsigned int> indices, const char *texture_path);

  virtual void render();

  static void registerDx11DeviceAndDeviceContext(ID3D11Device *device, ID3D11DeviceContext *device_context);

protected:
  inline static ID3D11Device *device = NULL;
  inline static ID3D11DeviceContext *device_context = NULL;

  ID3D11Buffer *vertex_buffer; // Vertex buffer
  ID3D11Buffer *index_buffer;  // Index buffer

  ID3D11ShaderResourceView *p_texture_view = NULL;

  size_t vertex_count;
  size_t index_count;

  void initialize(std::vector<vertex_t> vertices, std::vector<unsigned int> indices);
};
