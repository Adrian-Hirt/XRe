#include <xre/texture.h>

// Stb includes
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const std::string &path, std::shared_ptr<VulkanHandler> vulkan_handler) : m_vulkan_handler(vulkan_handler) {
  VkImage texture_image = createTextureImage(path);
  createTextureImageView(texture_image);
  createTextureSampler();
}

VkImage Texture::createTextureImage(const std::string &path) {
  VkResult result;

  // Load the image with the STB image library
  int texture_width, texture_height, texture_channels;
  stbi_uc *pixels = stbi_load(path.c_str(), &texture_width, &texture_height, &texture_channels, STBI_rgb_alpha);
  VkDeviceSize image_size = texture_width * texture_height * 4;

  if (!pixels) {
    Utils::exitWithMessage("failed to load texture image!");
  }

  // Create our staging buffer
  Buffer staging_buffer =
      Buffer(m_vulkan_handler->getLogicalDevice(), m_vulkan_handler->getPhysicalDevice(), image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

  // Load pixels into buffer
  staging_buffer.loadData(pixels);

  // Clean up the original pixel array
  stbi_image_free(pixels);

  // Setup the struct to create the image in Vulkan
  VkImageCreateInfo image_create_info{};
  image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_create_info.imageType = VK_IMAGE_TYPE_2D;
  image_create_info.extent.width = static_cast<uint32_t>(texture_width);
  image_create_info.extent.height = static_cast<uint32_t>(texture_height);
  image_create_info.extent.depth = 1;
  image_create_info.mipLevels = 1;
  image_create_info.arrayLayers = 1;
  image_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
  image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_create_info.flags = 0; // Optional

  // Create the image
  VkImage texture_image;
  result = vkCreateImage(m_vulkan_handler->getLogicalDevice(), &image_create_info, nullptr, &texture_image);
  Utils::checkVkResult(result, "failed to create image!");

  // Get the memory requirements from the runtime
  VkMemoryRequirements memory_requirements;
  vkGetImageMemoryRequirements(m_vulkan_handler->getLogicalDevice(), texture_image, &memory_requirements);

  // Setup the info struct to allocate memory
  VkMemoryAllocateInfo allocate_info{};
  allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocate_info.allocationSize = memory_requirements.size;
  allocate_info.memoryTypeIndex = VulkanUtils::findMemoryType(m_vulkan_handler->getPhysicalDevice(), memory_requirements.memoryTypeBits,
                                                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  // Allocate and bind the image memory
  VkDeviceMemory texture_image_memory;
  result = vkAllocateMemory(m_vulkan_handler->getLogicalDevice(), &allocate_info, nullptr, &texture_image_memory);
  Utils::checkVkResult(result, "failed to allocate image memory!");

  result = vkBindImageMemory(m_vulkan_handler->getLogicalDevice(), texture_image, texture_image_memory, 0);
  Utils::checkVkResult(result, "failed to bing image memory!");

  // Transition the image layout to the correct layout VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, then copy
  // the buffer to the image, and then transition the image to the final layout VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL.
  transitionImageLayout(texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  copyBufferToImage(staging_buffer.getBuffer(), texture_image, static_cast<uint32_t>(texture_width), static_cast<uint32_t>(texture_height));
  transitionImageLayout(texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  // Cleanup
  staging_buffer.destroy();

  return texture_image;
}

void Texture::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout) {
  VkCommandBuffer command_buffer = m_vulkan_handler->beginSingleTimeCommands();

  VkPipelineStageFlags source_stage;
  VkPipelineStageFlags destination_stage;

  // Setup the barrier which we'll use to transition the image layout
  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = old_layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    Utils::exitWithMessage("unsupported layout transition!");
  }

  vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

  m_vulkan_handler->endSingleTimeCommands(command_buffer);
}

void Texture::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
  VkCommandBuffer command_buffer = m_vulkan_handler->beginSingleTimeCommands();

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width, height, 1};

  vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  m_vulkan_handler->endSingleTimeCommands(command_buffer);
}

void Texture::createTextureImageView(VkImage image) {
  VkResult result;

  VkImageViewCreateInfo image_view_create_info{};
  image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_create_info.image = image;
  image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  image_view_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
  image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_view_create_info.subresourceRange.baseMipLevel = 0;
  image_view_create_info.subresourceRange.levelCount = 1;
  image_view_create_info.subresourceRange.baseArrayLayer = 0;
  image_view_create_info.subresourceRange.layerCount = 1;

  result = vkCreateImageView(m_vulkan_handler->getLogicalDevice(), &image_view_create_info, nullptr, &m_texture_image_view);
  Utils::checkVkResult(result, "failed to create texture image view!");
}

void Texture::createTextureSampler() {
  VkResult result;

  VkSamplerCreateInfo sampler_create_info{};
  sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_create_info.magFilter = VK_FILTER_LINEAR;
  sampler_create_info.minFilter = VK_FILTER_LINEAR;
  sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_create_info.anisotropyEnable = VK_TRUE;
  sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  sampler_create_info.unnormalizedCoordinates = VK_FALSE;
  sampler_create_info.compareEnable = VK_FALSE;
  sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
  sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_create_info.mipLodBias = 0.0f;
  sampler_create_info.minLod = 0.0f;
  sampler_create_info.maxLod = 0.0f;

  // Check the max level of anisotropic filtering that the physical device supports
  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(m_vulkan_handler->getPhysicalDevice(), &properties);
  sampler_create_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

  result = vkCreateSampler(m_vulkan_handler->getLogicalDevice(), &sampler_create_info, nullptr, &m_texture_sampler);
  Utils::checkVkResult(result, "failed to create texture sampler!");
}

VkImageView Texture::getTextureImageView() { return m_texture_image_view; }

VkSampler Texture::getTextureSampler() { return m_texture_sampler; }
