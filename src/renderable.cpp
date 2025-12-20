#include <xre/renderable.h>

// Function to initialize the "common" data of a mesh, to avoid code-duplication
void Renderable::initialize(std::vector<Vertex> vertices, std::vector<uint16_t> indices) {
  // Check that the device and physical device have been set
  Utils::checkBoolResult(s_device != nullptr, "Device has not been set!");
  Utils::checkBoolResult(s_physical_device != nullptr, "Physical device has not been set");

  // Store number of vertices and indices
  m_vertex_count = vertices.size();
  m_index_count = indices.size();

  // Create vertex buffer
  size_t size = sizeof(Vertex) * vertices.size();
  m_vertex_buffer = new Buffer(
    s_device,
    s_physical_device,
    static_cast<VkDeviceSize>(size),
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
  );
  m_vertex_buffer->loadData(vertices);

  // Create index buffer
  size_t index_size = sizeof(uint16_t) * indices.size();
  m_index_buffer = new Buffer(
    s_device,
    s_physical_device,
    static_cast<VkDeviceSize>(index_size),
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT
  );
  m_index_buffer->loadData(indices);

  if (hasBoundingBox()) {
    // Store vertex positions temporary
    std::vector<glm::vec3> vertex_positions;

    for(int i = 0; i < m_vertex_count; i++) {
      vertex_positions.push_back(vertices[i].position);
    }

    // Create the bounding box for this mesh
    m_bounding_box = OOBB(vertex_positions);

    // Store the vertices from the OOBB
    auto bounding_box_corners = m_bounding_box.getCorners();
    std::vector<Vertex> bbox_vertices;

    // Create vertices
    for (auto corner : bounding_box_corners) {
      Vertex vert;
      vert.position = corner;
      bbox_vertices.push_back(vert);
    }

    // Create vertex buffer for object oriented bounding boxes.
    size_t size = sizeof(Vertex) * bbox_vertices.size();
    m_bounding_box_vertex_buffer = new Buffer(
      s_device,
      s_physical_device,
      static_cast<VkDeviceSize>(size),
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    );
    m_bounding_box_vertex_buffer->loadData(bbox_vertices);
    m_bbox_index_count = m_bounding_box.getLineIndices().size();

    // Create index buffer for object oriented bounding boxes.
    size_t index_size = sizeof(uint16_t) * m_bbox_index_count;
    m_bounding_box_index_buffer = new Buffer(
      s_device,
      s_physical_device,
      static_cast<VkDeviceSize>(index_size),
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT
    );
    m_bounding_box_index_buffer->loadData(m_bounding_box.getLineIndices());
  }
}

void Renderable::registerDeviceAndPhysicalDevice(VkDevice device, VkPhysicalDevice physical_device) {
  Renderable::s_device = device;
  Renderable::s_physical_device = physical_device;
}

// DirectX::BoundingOrientedBox Renderable::getBoundingBox() {
//   return m_bounding_box;
// }

void Renderable::render(RenderContext& ctx) {
  //------------------------------------------------------------------------------------------------------
  // Bind buffers
  //------------------------------------------------------------------------------------------------------
  const VkDeviceSize offset = 0u;
  const VkBuffer vertex_buffer = m_vertex_buffer->getBuffer();
  vkCmdBindVertexBuffers(ctx.command_buffer, 0u, 1u, &vertex_buffer, &offset);

  // Bind the index buffer
  const VkBuffer index_buffer = m_index_buffer->getBuffer(); // Your new index buffer
  vkCmdBindIndexBuffer(ctx.command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT16);

  //------------------------------------------------------------------------------------------------------
  // Draw
  //------------------------------------------------------------------------------------------------------
  // Draw using indices
  vkCmdDrawIndexed(ctx.command_buffer, m_index_count, 1u, 0u, 0u, 0u);
}

void Renderable::renderBoundingBox(RenderContext& ctx) {
  //------------------------------------------------------------------------------------------------------
  // Bind buffers for bounding boxes
  //------------------------------------------------------------------------------------------------------
  const VkDeviceSize offset = 0u;
  const VkBuffer bbox_vertex_buffer = m_bounding_box_vertex_buffer->getBuffer();
  vkCmdBindVertexBuffers(ctx.command_buffer, 0u, 1u, &bbox_vertex_buffer, &offset);

  // Bind the index buffer
  const VkBuffer bbox_index_buffer = m_bounding_box_index_buffer->getBuffer(); // Your new index buffer
  vkCmdBindIndexBuffer(ctx.command_buffer, bbox_index_buffer, 0, VK_INDEX_TYPE_UINT16);

  //------------------------------------------------------------------------------------------------------
  // Draw
  //------------------------------------------------------------------------------------------------------
  // Draw using indices
  vkCmdDrawIndexed(ctx.command_buffer, m_bbox_index_count, 1u, 0u, 0u, 0u);
}

OOBB Renderable::getObjectOrientedBoundingBox() {
  return m_bounding_box;
}

