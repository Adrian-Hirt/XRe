#pragma once

// DirectX includes
#include <d3d11.h>
#include <DirectXTex/WICTextureLoader11.h>
#include <DirectXMath/DirectXCollision.h>

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

  // vertex and index buffers
  ID3D11Buffer *vertex_buffer;
  ID3D11Buffer *index_buffer;

  // Vertex and index buffer for bounding volumes, which mainly are used
  // for debugging purposes.
  ID3D11Buffer *bounding_box_vertex_buffer;
  ID3D11Buffer *bounding_box_index_buffer;

  // The applied texture
  ID3D11ShaderResourceView *p_texture_view = NULL;

  // Number of vertices and indices
  size_t vertex_count;
  size_t index_count;

  // The bounding box of this mesh
  DirectX::BoundingOrientedBox bounding_box;

  void initialize(std::vector<vertex_t> vertices, std::vector<unsigned int> indices);
};
