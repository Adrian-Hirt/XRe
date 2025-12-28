#include <xre/resource_manager.h>

ResourceManager::ResourceManager(std::shared_ptr<VulkanHandler> vulkan_handler) : m_vulkan_handler(vulkan_handler) {}

std::shared_ptr<Texture> ResourceManager::texture(const std::string &path) { return std::make_shared<Texture>(path, m_vulkan_handler); }

std::shared_ptr<Material> ResourceManager::material(const std::string &vert_path, const std::string &frag_path) {
  return std::make_shared<Material>(vert_path, frag_path, m_vulkan_handler);
}

std::shared_ptr<Material> ResourceManager::material(const std::string &vert_path, const std::string &frag_path,
                                                    std::shared_ptr<Texture> texture) {
  return std::make_shared<Material>(vert_path, frag_path, texture, m_vulkan_handler);
}

std::shared_ptr<Model> ResourceManager::cube(std::shared_ptr<Material> material) { return cube(material, DEFAULT_MODEL_COLOR); }

std::shared_ptr<Model> ResourceManager::cube(std::shared_ptr<Material> material, glm::vec3 color) {
  return ModelFactory::createCube(color, material, m_vulkan_handler);
}

std::shared_ptr<Model> ResourceManager::sphere(std::shared_ptr<Material> material) { return sphere(material, DEFAULT_MODEL_COLOR); }

std::shared_ptr<Model> ResourceManager::sphere(std::shared_ptr<Material> material, glm::vec3 color) {
  return ModelFactory::createSphere(material, color, m_vulkan_handler);
}

std::shared_ptr<Model> ResourceManager::plane(float extent, std::shared_ptr<Material> material) {
  return plane(extent, material, DEFAULT_MODEL_COLOR);
}

std::shared_ptr<Model> ResourceManager::plane(float extent, std::shared_ptr<Material> material, glm::vec3 color) {
  return ModelFactory::createPlane(extent, material, color, m_vulkan_handler);
}

std::shared_ptr<Model> ResourceManager::quad(std::shared_ptr<Material> material) {
  return ModelFactory::createQuad(material, DEFAULT_MODEL_COLOR, m_vulkan_handler);
}

std::shared_ptr<Line> ResourceManager::line(float thickness, float length, std::shared_ptr<Material> material) {
  return line(thickness, length, material, DEFAULT_MODEL_COLOR);
}

std::shared_ptr<Line> ResourceManager::line(float thickness, float length, std::shared_ptr<Material> material, glm::vec3 color) {
  return std::make_shared<Line>(thickness, length, color, material, m_vulkan_handler);
}

std::shared_ptr<Model> ResourceManager::model(const char *model_path, std::shared_ptr<Material> material) {
  return model(model_path, material, DEFAULT_MODEL_COLOR);
}

std::shared_ptr<Model> ResourceManager::model(const char *model_path, std::shared_ptr<Material> material, glm::vec3 color) {
  return std::make_shared<Model>(model_path, color, material, m_vulkan_handler);
}

std::shared_ptr<Text> ResourceManager::text(std::string sentence) {
  return std::make_shared<Text>(sentence, m_vulkan_handler);
}
