#include <xre/scene_manager.h>

SceneManager::SceneManager(std::shared_ptr<VulkanHandler> vulkan_handler) {
  m_vulkan_handler = vulkan_handler;
}

void SceneManager::registerScene(std::string name, SceneFactory factory) {
  m_scene_factories[std::move(name)] = std::move(factory);
}

void SceneManager::setActive(const std::string& name) {
  auto found_scene = m_scene_factories.find(name);
  if (found_scene == m_scene_factories.end()) {
    Utils::exitWithMessage("Scene not registered: " + name);
  }

  // Deactivate old scene
  if (m_active_scene) {
    m_active_scene->onDeactivate();
    m_vulkan_handler->resetDescriptorPool();
  }

  // Create fresh instance and activate it
  m_active_scene = found_scene->second();
  m_active_scene->onActivate();
}

void SceneManager::updateSimulation(XrTime predicted_time) {
  if (m_active_scene) {
    m_active_scene->updateSimulation(predicted_time);
  }
}

void SceneManager::draw(RenderContext& ctx) {
  if (m_active_scene) {
    m_active_scene->draw(ctx);
  }
}