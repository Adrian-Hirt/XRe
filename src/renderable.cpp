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
  size_t index_size = sizeof(Vertex) * indices.size();
  m_index_buffer = new Buffer(
    s_device,
    s_physical_device,
    static_cast<VkDeviceSize>(index_size),
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT
  );
  m_index_buffer->loadData(indices);

//   if (hasBoundingBox()) {
//     // Store vertex positions temporary
//     DirectX::XMFLOAT3 vertex_positions[m_vertex_count];

//     for(int i = 0; i < m_vertex_count; i++) {
//       vertex_positions[i] = vertices[i].coordinates;
//     }

//     // Create the bounding box for this mesh
//     DirectX::BoundingOrientedBox::CreateFromPoints(m_bounding_box, m_vertex_count, vertex_positions, sizeof(DirectX::XMFLOAT3));
//   }
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
  // Update uniform buffer
  //------------------------------------------------------------------------------------------------------
  ctx.uniform_buffer_object.world = glm::translate(glm::mat4(1.0f), { 10.0f, 0.0f, 0.0f });
  ctx.uniform_buffer->loadData(ctx.uniform_buffer_object);

  //------------------------------------------------------------------------------------------------------
  // Bind buffers
  //------------------------------------------------------------------------------------------------------
  const VkDeviceSize offset = 0u;
  const VkBuffer vertexBuffer = m_vertex_buffer->getBuffer();
  vkCmdBindVertexBuffers(ctx.command_buffer, 0u, 1u, &vertexBuffer, &offset);

  // Bind the index buffer
  const VkBuffer indexBuffer = m_index_buffer->getBuffer(); // Your new index buffer
  vkCmdBindIndexBuffer(ctx.command_buffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

  //------------------------------------------------------------------------------------------------------
  // Draw
  //------------------------------------------------------------------------------------------------------
  // Draw using indices
  vkCmdDrawIndexed(ctx.command_buffer, m_index_count, 1u, 0u, 0u, 0u);
}
