#include <xre/resource_manager.h>

ResourceManager::ResourceManager(VulkanHandler &vulkan_handler) : m_vulkan_handler(vulkan_handler) { }

Texture ResourceManager::texture(const std::string& path) {
  return Texture(path, m_vulkan_handler);
}
