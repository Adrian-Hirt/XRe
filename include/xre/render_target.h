#pragma once

// Vulkan includes
#include <vulkan/vulkan.h>

// XRe includes
#include <xre/utils.h>

class RenderTarget final {
public:
  RenderTarget(VkDevice device, VkImage image, VkExtent2D size, VkFormat format, VkRenderPass renderPass);

  void destroy();

  VkImage getImage();
  VkFramebuffer getFramebuffer();

private:
  VkDevice m_device = nullptr;
  VkImage m_image = nullptr;
  VkImageView m_image_view = nullptr;
  VkFramebuffer m_framebuffer = nullptr;
};