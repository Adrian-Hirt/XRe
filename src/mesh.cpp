#include <xre/mesh.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::shared_ptr<VulkanHandler> vulkan_handler) {
  // Call general initialize method
  initialize(vertices, indices, vulkan_handler);
}

void Mesh::render(RenderContext &ctx) { Renderable::render(ctx); }
