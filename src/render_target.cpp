#include <xre/render_target.h>

RenderTarget::RenderTarget(VkDevice device, VkImage image, VkExtent2D size, VkFormat format, VkRenderPass renderPass) : m_device(device), m_image(image) {
  VkResult result;
  
  // Create image view
  VkImageViewCreateInfo image_view_create_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
  image_view_create_info.image = image;
  image_view_create_info.format = format;
  image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_view_create_info.subresourceRange.baseArrayLayer = 0u;
  image_view_create_info.subresourceRange.baseMipLevel = 0u;
  image_view_create_info.subresourceRange.layerCount = 1u;
  image_view_create_info.subresourceRange.levelCount = 1u;
  result = vkCreateImageView(device, &image_view_create_info, nullptr, &m_image_view);
  Utils::checkVkResult(result, "Failed to create image view for render target");

  // Create framebuffer
  VkFramebufferCreateInfo framebuffer_create_info{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
  framebuffer_create_info.renderPass = renderPass;
  framebuffer_create_info.attachmentCount = 1u;
  framebuffer_create_info.pAttachments = &m_image_view;
  framebuffer_create_info.width = size.width;
  framebuffer_create_info.height = size.height;
  framebuffer_create_info.layers = 1u;
  result = vkCreateFramebuffer(device, &framebuffer_create_info, nullptr, &m_framebuffer);
  Utils::checkVkResult(result, "Failed to create framebuffer for render target");
}

void RenderTarget::destroy() {
  vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
  vkDestroyImageView(m_device, m_image_view, nullptr);
}

VkImage RenderTarget::getImage() {
  return m_image;
}

VkFramebuffer RenderTarget::getFramebuffer() {
  return m_framebuffer;
}
