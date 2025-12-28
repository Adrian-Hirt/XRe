#include <xre/resource_manager.h>

ResourceManager::ResourceManager(std::shared_ptr<VulkanHandler> vulkan_handler) : m_vulkan_handler(vulkan_handler) { }

std::shared_ptr<Texture> ResourceManager::texture(const std::string& path) {
  return std::make_shared<Texture>(path, m_vulkan_handler);
}

std::shared_ptr<Material> ResourceManager::material(const std::string& vert_path, const std::string& frag_path) {
  return std::make_shared<Material>(vert_path, frag_path, m_vulkan_handler);
}

std::shared_ptr<Material> ResourceManager::material(const std::string& vert_path, const std::string& frag_path, std::shared_ptr<Texture> texture) {
  return std::make_shared<Material>(vert_path, frag_path, texture, m_vulkan_handler);
}

std::shared_ptr<Model> ResourceManager::cube(glm::vec3 color, std::shared_ptr<Material> material) {
  return ModelFactory::createCube(color, material, m_vulkan_handler);
}

std::shared_ptr<Model> ResourceManager::sphere(std::shared_ptr<Material> material) {
  return ModelFactory::createSphere(material, m_vulkan_handler);
}

std::shared_ptr<Model> ResourceManager::plane(float extent, std::shared_ptr<Material> material) {
  return ModelFactory::createPlane(extent, material, m_vulkan_handler);
}
