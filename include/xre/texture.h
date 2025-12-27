#pragma once

// Vulkan includes
#include <vulkan/vulkan.h>

// XRe includes
#include <xre/vulkan_handler.h>
#include <xre/buffer.h>
#include <xre/vulkan_utils.h>

// Other includes
#include <string>

class Texture {
public:
  Texture(const std::string& path);

  // TODO: replace this by a more sensible architecture
  static void registerVulkanHandler(VulkanHandler handler);

  VkImageView getTextureImageView();
  VkSampler getTextureSampler();

private:
  inline static VulkanHandler s_vulkan_handler;

  void createTextureImage(const std::string& path);
  void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
  void createTextureImageView();
  void createTextureSampler();

  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;
};
