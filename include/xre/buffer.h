#pragma once

#include <vulkan/vulkan.h>
#include "utils.h"

class Buffer {
public:
  Buffer(VkDevice device, VkPhysicalDevice physical_device, VkDeviceSize size, VkBufferUsageFlags buffer_usage_flags);

  void destroy();

  void* map() const;
  void unmap() const;
  VkBuffer getBuffer() const;

private:
  VkDevice m_device = nullptr;
  VkBuffer m_buffer = nullptr;
  VkDeviceMemory m_device_memory = nullptr;
  VkDeviceSize m_size = 0u;
};