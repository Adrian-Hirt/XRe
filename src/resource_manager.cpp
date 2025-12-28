#include <xre/resource_manager.h>

ResourceManager::ResourceManager(std::shared_ptr<VulkanHandler> vulkan_handler) : m_vulkan_handler(vulkan_handler) { }

Texture ResourceManager::texture(const std::string& path) {
  return Texture(path, m_vulkan_handler);
}

std::shared_ptr<Material> ResourceManager::material(const std::string& vert_path, const std::string& frag_path) {
  return std::make_shared<Material>(vert_path, frag_path, m_vulkan_handler);
}

std::shared_ptr<Material> ResourceManager::material(const std::string& vert_path, const std::string& frag_path, Texture texture) {
  return std::make_shared<Material>(vert_path, frag_path, texture, m_vulkan_handler);
}

Model ResourceManager::cube(glm::vec3 color, std::shared_ptr<Material> material) {
  return ModelFactory::createCube(color, material, m_vulkan_handler);
}
