#pragma once

// XRe includes
#include <xre/vulkan_handler.h>
#include <xre/texture.h>

// Other includes
#include <memory>

class ResourceManager {
public:
  ResourceManager(std::shared_ptr<VulkanHandler> vulkan_handler);

  Texture texture(const std::string& path);

private:
  std::shared_ptr<VulkanHandler> m_vulkan_handler;
};
