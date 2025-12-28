#include <xre/mesh.h>

//------------------------------------------------------------------------------------------------------
// Initialize the mesh.
// Arguments:
//  1) Vector with the vertices of the mesh
//  2) Vector with the incides of the mesh
//------------------------------------------------------------------------------------------------------
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices) {
  // Call general initialize method
  initialize(vertices, indices);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::shared_ptr<VulkanHandler> vulkan_handler) {
  // Call general initialize method
  initialize(vertices, indices, vulkan_handler);
}

void Mesh::render(RenderContext &ctx) { Renderable::render(ctx); }
