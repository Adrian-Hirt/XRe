#pragma once

// Vulkan includes
#include <vulkan/vulkan.h>

// XRe includes
#include <xre/vulkan_handler.h>

// Other includes
#include <string>


class Texture {
public:
  Texture(const std::string& path);

  // TODO: replace this by a more sensible architecture
  static void registerVulkanHandler(VulkanHandler handler);

private:
  inline static VulkanHandler s_vulkan_handler;

};
