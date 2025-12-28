#pragma once

// Vulkan includes
#include <vulkan/vulkan.h>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/vulkan_utils.h>

// Other includes
#include <stb_image.h>

class Buffer {
public:
  Buffer(VkDevice device, VkPhysicalDevice physical_device, VkDeviceSize size, VkBufferUsageFlags buffer_usage_flags);

  void destroy();
  VkBuffer getBuffer();
  void loadData(std::vector<Vertex> input);
  void loadData(std::vector<uint16_t> input);
  void loadData(ModelUniformBufferObject input, VkDeviceSize offset);
  void loadData(GlobalUniformBufferObject input);
  void loadData(stbi_uc *input);

private:
  VkDevice m_device = nullptr;
  VkBuffer m_buffer = nullptr;
  VkDeviceMemory m_device_memory = nullptr;
  VkDeviceSize m_size = 0u;

  void *map();
  void unmap();
};