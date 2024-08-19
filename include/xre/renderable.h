#pragma once

// DirectX includes
#include <d3d11.h>
#include <DirectXMath/DirectXMath.h>
#include <DirectXTex/WICTextureLoader11.h>
#include <DirectXMath/DirectXCollision.h>

// Include vector header
#include <vector>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/shader.h>

// Base class which is subclassed by other classes that are "renderable", i.e.
// can be rendered to display some output in the program.
class Renderable {
public:
  virtual void render() {};
  DirectX::BoundingOrientedBox getBoundingBox();

  static void registerDx11DeviceAndDeviceContext(ID3D11Device *device, ID3D11DeviceContext *device_context);

protected:
  inline static ID3D11Device *s_device = NULL;
  inline static ID3D11DeviceContext *s_device_context = NULL;
  inline static ID3D11ShaderResourceView *s_nulltexture = NULL;

  // Any renderable has a bounding box by default. Subclasses which do not
  // can override this method to disable bounding boxes.
  virtual inline bool hasBoundingBox() { return true; };

  // Whether the mesh should be changeable (i.e. the vertex and index buffers need
  // to be updateable or not).
  virtual inline bool usesStaticBuffers() { return true; };

  // vertex and index buffers
  ID3D11Buffer *m_vertex_buffer;
  ID3D11Buffer *m_index_buffer;

  // Vertex and index buffer for bounding volumes, which mainly are used
  // for debugging purposes.
  ID3D11Buffer *m_bounding_box_vertex_buffer;
  ID3D11Buffer *m_bounding_box_index_buffer;

  // The applied texture
  ID3D11ShaderResourceView *m_texture_view = NULL;

  // Number of vertices and indices
  size_t m_vertex_count;
  size_t m_index_count;

  // The bounding box of this mesh
  DirectX::BoundingOrientedBox m_bounding_box;

  // Whether the mesh should be changeable (i.e. the vertex and index buffers need
  // to be updateable or not).
  bool m_static_buffers = true;

  void initialize(std::vector<vertex_t> vertices, std::vector<unsigned int> indices);
  void createVertexBuffer(std::vector<vertex_t> data, ID3D11Buffer **target_buffer);
  void createIndexBuffer(std::vector<unsigned int> data, ID3D11Buffer **target_buffer);
};
