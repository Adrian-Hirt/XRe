#include <xre/render_target.h>

RenderTarget::RenderTarget(VkDevice device, VkPhysicalDevice physical_device, VkImage color_image, VkExtent2D size, VkFormat color_format,
                           VkRenderPass render_pass)
    : m_device(device), m_color_image(color_image) {
  VkResult result;

  // Create color image view
  VkImageViewCreateInfo image_view_create_info{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
  image_view_create_info.image = color_image;
  image_view_create_info.format = color_format;
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
  result = vkCreateImageView(device, &image_view_create_info, nullptr, &m_color_image_view);
  Utils::checkVkResult(result, "Failed to create image view for render target");

  // Create depth image
  VkFormat depth_format = VK_FORMAT_D32_SFLOAT;
  VkImageCreateInfo depth_image_create_info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
  depth_image_create_info.imageType = VK_IMAGE_TYPE_2D;
  depth_image_create_info.extent = {size.width, size.height, 1};
  depth_image_create_info.mipLevels = 1;
  depth_image_create_info.arrayLayers = 1;
  depth_image_create_info.format = depth_format;
  depth_image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  depth_image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
  depth_image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  vkCreateImage(device, &depth_image_create_info, nullptr, &m_depth_image);

  // Allocate and bind memory
  VkMemoryRequirements memReq;
  vkGetImageMemoryRequirements(device, m_depth_image, &memReq);
  VkMemoryAllocateInfo allocInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
  allocInfo.allocationSize = memReq.size;
  allocInfo.memoryTypeIndex = VulkanUtils::findMemoryType(physical_device, memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vkAllocateMemory(device, &allocInfo, nullptr, &m_depth_memory);
  vkBindImageMemory(device, m_depth_image, m_depth_memory, 0);

  // Create the depth view
  VkImageViewCreateInfo depthViewInfo{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
  depthViewInfo.image = m_depth_image;
  depthViewInfo.format = depth_format;
  depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  depthViewInfo.subresourceRange.levelCount = 1;
  depthViewInfo.subresourceRange.layerCount = 1;
  vkCreateImageView(device, &depthViewInfo, nullptr, &m_depth_image_view);

  // Create framebuffer
  std::array<VkImageView, 2> attachments = {m_color_image_view, m_depth_image_view};
  VkFramebufferCreateInfo framebuffer_create_info{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
  framebuffer_create_info.renderPass = render_pass;
  framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
  ;
  framebuffer_create_info.pAttachments = attachments.data();
  framebuffer_create_info.width = size.width;
  framebuffer_create_info.height = size.height;
  framebuffer_create_info.layers = 1u;
  result = vkCreateFramebuffer(device, &framebuffer_create_info, nullptr, &m_framebuffer);
  Utils::checkVkResult(result, "Failed to create framebuffer for render target");
}

void RenderTarget::destroy() {
  vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
  // vkDestroyImageView(m_device, m_image_view, nullptr);
}

VkImage RenderTarget::getImage() { return m_color_image; }

VkFramebuffer RenderTarget::getFramebuffer() { return m_framebuffer; }
