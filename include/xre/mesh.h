#pragma once

// Other includes
#include <vector>
#include <memory>

// GLM includes
#include <glm/glm/vec3.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

// XRe includes
// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/buffer.h>
#include <xre/object_oriented_bounding_box.h>
#include <xre/vulkan_handler.h>

class Mesh {
public:
  Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::shared_ptr<VulkanHandler> vulkan_handler);

  OOBB getObjectOrientedBoundingBox();
  const std::vector<glm::vec3> getVectorPositions() const;
private:
  void render(RenderContext &ctx);
  void renderBoundingBox(RenderContext &ctx);
  void initialize(std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::shared_ptr<VulkanHandler> vulkan_handler);

  // Any mesh has a bounding box by default.
  virtual inline bool hasBoundingBox() { return true; };

  std::shared_ptr<VulkanHandler> m_vulkan_handler;

  // vertex and index buffers
  Buffer *m_vertex_buffer = nullptr;
  Buffer *m_index_buffer = nullptr;

  // Vertex positions
  std::vector<glm::vec3> m_vertex_positions;

  // Vertex and index buffer for bounding volumes, which mainly are used
  // for debugging purposes.
  Buffer *m_bounding_box_vertex_buffer = nullptr;
  Buffer *m_bounding_box_index_buffer = nullptr;

  // Number of vertices and indices
  size_t m_vertex_count;
  size_t m_index_count;
  size_t m_bbox_index_count;

  // The bounding box of this mesh
  OOBB m_bounding_box;

  // Only Model can call Mesh::render()
  friend class Model;
};
