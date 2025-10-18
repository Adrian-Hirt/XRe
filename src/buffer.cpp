#include <xre/buffer.h>

Buffer::Buffer(VkDevice device, VkPhysicalDevice physical_device, VkDeviceSize size, VkBufferUsageFlags buffer_usage_flags) : m_device(device), m_size(size) {
  VkResult result;

  VkBufferCreateInfo buffer_create_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
  buffer_create_info.size = size;
  buffer_create_info.usage = buffer_usage_flags;
  buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  result = vkCreateBuffer(device, &buffer_create_info, nullptr, &m_buffer);
  Utils::checkVkResult(result, "Failed to create buffer");

  VkMemoryRequirements memory_requirements;
  vkGetBufferMemoryRequirements(device, m_buffer, &memory_requirements);

  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

  const uint32_t properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  const VkMemoryPropertyFlags type_filter = memory_requirements.memoryTypeBits;
  uint32_t memoryTypeIndex = 0u;
  bool memory_type_found = false;
  for (uint32_t i = 0u; i < memory_properties.memoryTypeCount; ++i) {
    const VkMemoryPropertyFlags property_flags = memory_properties.memoryTypes[i].propertyFlags;
    if (type_filter & (1 << i) && (property_flags & properties) == properties) {
      memoryTypeIndex = i;
      memory_type_found = true;
      break;
    }
  }

  Utils::checkBoolResult(memory_type_found, "Failed to find suitable memory type for buffer");

  VkMemoryAllocateInfo memory_allocate_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
  memory_allocate_info.allocationSize = memory_requirements.size;
  memory_allocate_info.memoryTypeIndex = memoryTypeIndex;
  result = vkAllocateMemory(device, &memory_allocate_info, nullptr, &m_device_memory);
  Utils::checkVkResult(result, "Failed to allocate memory for buffer");

  result = vkBindBufferMemory(device, m_buffer, m_device_memory, 0u);
  Utils::checkVkResult(result, "Failed to allocate bind memory for buffer");
}

void Buffer::loadData(std::vector<Vertex> input) {
  void* data = map();
  uint32_t size = sizeof(Vertex) * input.size();
  memcpy(data, input.data(), size);
  unmap();
}

void Buffer::loadData(std::vector<uint16_t> input) {
  void* data = map();
  uint32_t size = sizeof(uint16_t) * input.size();
  memcpy(data, input.data(), size);
  unmap();
}

void Buffer::loadData(ModelUniformBufferObject input, VkDeviceSize offset) {
  void* data = map();
  // Copy at given offset
  memcpy(static_cast<uint8_t*>(data) + offset, &input, sizeof(input));
  unmap();
}

void Buffer::loadData(GlobalUniformBufferObject input) {
  void* data = map();
  memcpy(data, &input, sizeof(input));
  unmap();
}

void Buffer::destroy() {
  vkFreeMemory(m_device, m_device_memory, nullptr);
  vkDestroyBuffer(m_device, m_buffer, nullptr);
}

void* Buffer::map() {
  void* data;
  VkResult result = vkMapMemory(m_device, m_device_memory, 0u, m_size, 0, &data);
  Utils::checkVkResult(result, "Failed to map memory for buffer");

  return data;
}

void Buffer::unmap() {
  vkUnmapMemory(m_device, m_device_memory);
}


VkBuffer Buffer::getBuffer() {
  return m_buffer;
}
