#pragma once

// XRe includes
#include <xre/vulkan_handler.h>
#include <xre/texture.h>
#include <xre/material.h>
#include <xre/model_factory.h>

// Other includes
#include <memory>

class ResourceManager {
public:
  ResourceManager(std::shared_ptr<VulkanHandler> vulkan_handler);

  Texture texture(const std::string& path);
  std::shared_ptr<Material> material(const std::string& vert_path, const std::string& frag_path);
  std::shared_ptr<Material> material(const std::string& vert_path, const std::string& frag_path, Texture texture);

  Model cube(glm::vec3 color, std::shared_ptr<Material> material);

private:
  std::shared_ptr<VulkanHandler> m_vulkan_handler;
};
