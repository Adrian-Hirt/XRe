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

  std::shared_ptr<Texture> texture(const std::string& path);

  // Methods to create materials
  std::shared_ptr<Material> material(const std::string& vert_path, const std::string& frag_path);
  std::shared_ptr<Material> material(const std::string& vert_path, const std::string& frag_path, std::shared_ptr<Texture> texture);

  // Methods to create models that we have predefined
  // TODO: return unique ptrs and handle ownership correctly
  std::shared_ptr<Model> cube(glm::vec3 color, std::shared_ptr<Material> material);
  std::shared_ptr<Model> sphere(std::shared_ptr<Material> material);
  std::shared_ptr<Model> plane(float extent, std::shared_ptr<Material> material);

private:
  std::shared_ptr<VulkanHandler> m_vulkan_handler;
};
