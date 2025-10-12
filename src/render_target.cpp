#include <xre/render_target.h>

RenderTarget::RenderTarget(VkDevice device, VkImage image, VkExtent2D size, VkFormat format, VkRenderPass renderPass) : m_device(device), m_image(image) {
  VkResult result;
  
  // Create image view
  VkImageViewCreateInfo imageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
  imageViewCreateInfo.image = image;
  imageViewCreateInfo.format = format;
  imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageViewCreateInfo.subresourceRange.baseArrayLayer = 0u;
  imageViewCreateInfo.subresourceRange.baseMipLevel = 0u;
  imageViewCreateInfo.subresourceRange.layerCount = 1u;
  imageViewCreateInfo.subresourceRange.levelCount = 1u;
  result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_image_view);
  Utils::checkVkResult(result, "Failed to create image view for render target");

  // Create framebuffer
  VkFramebufferCreateInfo framebufferCreateInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
  framebufferCreateInfo.renderPass = renderPass;
  framebufferCreateInfo.attachmentCount = 1u;
  framebufferCreateInfo.pAttachments = &m_image_view;
  framebufferCreateInfo.width = size.width;
  framebufferCreateInfo.height = size.height;
  framebufferCreateInfo.layers = 1u;
  result = vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &m_framebuffer);
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
