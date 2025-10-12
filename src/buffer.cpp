#include <xre/buffer.h>

Buffer::Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags bufferUsageFlags) : m_device(device), m_size(size) {
  VkResult result;

  VkBufferCreateInfo bufferCreateInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
  bufferCreateInfo.size = size;
  bufferCreateInfo.usage = bufferUsageFlags;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &m_buffer);
  Utils::checkVkResult(result, "Failed to create buffer");

  VkMemoryRequirements memoryRequirements;
  vkGetBufferMemoryRequirements(device, m_buffer, &memoryRequirements);

  VkPhysicalDeviceMemoryProperties memoryProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

  const uint32_t properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  const VkMemoryPropertyFlags typeFilter = memoryRequirements.memoryTypeBits;
  uint32_t memoryTypeIndex = 0u;
  bool memoryTypeFound = false;
  for (uint32_t i = 0u; i < memoryProperties.memoryTypeCount; ++i) {
    const VkMemoryPropertyFlags propertyFlags = memoryProperties.memoryTypes[i].propertyFlags;
    if (typeFilter & (1 << i) && (propertyFlags & properties) == properties) {
      memoryTypeIndex = i;
      memoryTypeFound = true;
      break;
    }
  }

  Utils::checkBoolResult(memoryTypeFound, "Failed to find suitable memory type for buffer");

  VkMemoryAllocateInfo memoryAllocateInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
  memoryAllocateInfo.allocationSize = memoryRequirements.size;
  memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
  result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &m_device_memory);
  Utils::checkVkResult(result, "Failed to allocate memory for buffer");

  result = vkBindBufferMemory(device, m_buffer, m_device_memory, 0u);
  Utils::checkVkResult(result, "Failed to allocate bind memory for buffer");
}

void Buffer::destroy() {
  vkFreeMemory(m_device, m_device_memory, nullptr);
  vkDestroyBuffer(m_device, m_buffer, nullptr);
}

void* Buffer::map() const {
  void* data;
  VkResult result = vkMapMemory(m_device, m_device_memory, 0u, m_size, 0, &data);
  Utils::checkVkResult(result, "Failed to map memory for buffer");

  return data;
}

void Buffer::unmap() const {
  vkUnmapMemory(m_device, m_device_memory);
}


VkBuffer Buffer::getBuffer() const {
  return m_buffer;
}
