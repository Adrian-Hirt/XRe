#pragma once

// Include vector header
#include <vector>

// GLM includes
#include <glm/glm/vec3.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/buffer.h>
#include <xre/object_oriented_bounding_box.h>

// Base class which is subclassed by other classes that are "renderable", i.e.
// can be rendered to display some output in the program.
class Renderable {
public:
  static void registerDeviceAndPhysicalDevice(VkDevice device, VkPhysicalDevice physical_device);
  OOBB getObjectOrientedBoundingBox();

protected:
  inline static VkDevice s_device = VK_NULL_HANDLE;
  inline static VkPhysicalDevice s_physical_device = VK_NULL_HANDLE;

  // Any renderable has a bounding box by default. Subclasses which do not
  // can override this method to disable bounding boxes.
  virtual inline bool hasBoundingBox() { return true; };

  // Protected as only subclasses may use it
  virtual void render(RenderContext &ctx);
  void renderBoundingBox(RenderContext &ctx);

  // vertex and index buffers
  Buffer *m_vertex_buffer = nullptr;
  Buffer *m_index_buffer = nullptr;

  // Vertex and index buffer for bounding volumes, which mainly are used
  // for debugging purposes.
  Buffer *m_bounding_box_vertex_buffer = nullptr;
  Buffer *m_bounding_box_index_buffer = nullptr;

  // Number of vertices and indices
  size_t m_vertex_count;
  size_t m_index_count;
  size_t m_bbox_index_count;

  // The bounding box of this renderable
  OOBB m_bounding_box;

  void initialize(std::vector<Vertex> vertices, std::vector<uint16_t> indices);

  // Scene Node can call render() directly
  friend class SceneNode;
};
