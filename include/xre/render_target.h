#pragma once

// Vulkan includes
#include <vulkan/vulkan.h>

// XRe includes
#include <xre/utils.h>
#include <xre/vulkan_utils.h>

// Other includes
#include <array>

class RenderTarget final {
public:
  RenderTarget(VkDevice device, VkPhysicalDevice physicalDevice, VkImage image, VkExtent2D size, VkFormat format, VkRenderPass renderPass);

  void destroy();

  VkImage getImage();
  VkFramebuffer getFramebuffer();

private:
  VkDevice m_device = nullptr;
  VkImage m_color_image = nullptr;
  VkImage m_depth_image = nullptr;
  VkImageView m_color_image_view = nullptr;
  VkImageView m_depth_image_view = nullptr;
  VkFramebuffer m_framebuffer = nullptr;
  VkDeviceMemory m_depth_memory = nullptr;
};