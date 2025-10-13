#pragma once

// DirectX includes
// #include <d3d11.h>
// #include <DirectXMath/DirectXMath.h>
// #include <DirectXTex/WICTextureLoader11.h>
// #include <DirectXMath/DirectXCollision.h>

// Include vector header
#include <vector>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/buffer.h>
// #include <xre/shader.h>

// Base class which is subclassed by other classes that are "renderable", i.e.
// can be rendered to display some output in the program.
class Renderable {
public:
  Renderable(std::vector<Vertex> vertices, std::vector<uint16_t> indices);
  virtual void render(VkCommandBuffer command_buffer);
  // DirectX::BoundingOrientedBox getBoundingBox();

  static void registerDeviceAndPhysicalDevice(VkDevice device, VkPhysicalDevice physical_device);

protected:
  inline static VkDevice s_device = VK_NULL_HANDLE;
  inline static VkPhysicalDevice s_physical_device = VK_NULL_HANDLE;
  // inline static ID3D11ShaderResourceView *s_nulltexture = NULL;

  // Any renderable has a bounding box by default. Subclasses which do not
  // can override this method to disable bounding boxes.
  virtual inline bool hasBoundingBox() { return true; };

  // vertex and index buffers
  Buffer *m_vertex_buffer = nullptr;
  Buffer *m_index_buffer = nullptr; 

  // Vertex and index buffer for bounding volumes, which mainly are used
  // for debugging purposes.
  Buffer *m_bounding_box_vertex_buffer = nullptr;
  Buffer *m_bounding_box_index_buffer = nullptr;

  // // The applied texture
  // ID3D11ShaderResourceView *m_texture_view = NULL;

  // Number of vertices and indices
  size_t m_vertex_count;
  size_t m_index_count;

  // // The bounding box of this mesh
  // DirectX::BoundingOrientedBox m_bounding_box;

  // Shader m_shader;

  void initialize(std::vector<Vertex> vertices, std::vector<uint16_t> indices);
};
