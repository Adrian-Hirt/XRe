#pragma once

// Other includes
#include <vector>
#include <memory>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/renderable.h>
#include <xre/vulkan_handler.h>

class Mesh : public Renderable {
public:
  Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices);
  Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::shared_ptr<VulkanHandler> vulkan_handler);

private:
  void render(RenderContext &ctx);

  // Only Model can call Mesh::render()
  friend class Model;
};
