#pragma once

// XRe includes
#include <xre/vulkan_handler.h>
#include <xre/texture.h>

class ResourceManager {
public:
  ResourceManager(VulkanHandler &vulkan_handler);

  Texture texture(const std::string& path);

private:
  VulkanHandler& m_vulkan_handler;
};
