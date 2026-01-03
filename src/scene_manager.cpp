#include <xre/scene_manager.h>

void SceneManager::init(std::shared_ptr<VulkanHandler> vulkan_handler) {
  if (!s_instance) {
    s_instance = std::unique_ptr<SceneManager>(new SceneManager(vulkan_handler));
  }
}

SceneManager& SceneManager::instance() {
  if (!s_instance) {
    throw std::runtime_error("SceneManager not initialized!");
  }
  return *s_instance;
}

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

  // Keep old scene alive until we finish deactivation
  std::unique_ptr<Scene> old_scene = std::move(m_active_scene);

  // Create new scene
  m_active_scene = found_scene->second();

  // Deactivate old scene safely
  if (old_scene) {
    old_scene->onDeactivate();
    m_vulkan_handler->resetDescriptorPool();
  }

  // Activate new scene
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

void SceneManager::resetInteractionStates() {
  if (m_active_scene) {
    m_active_scene->resetInteractionStates();
  }
}

std::unordered_set<SceneNode *> SceneManager::getGrabbableNodeInstances() {
  if (m_active_scene) {
    return m_active_scene->getGrabbableNodeInstances();
  } else {
    return {};
  }
}

std::unordered_set<SceneNode *> SceneManager::getTerrainInstances() {
  if (m_active_scene) {
    return m_active_scene->getTerrainNodeInstances();
  } else {
    return {};
  }
}

void SceneManager::processButtonInteractions() {
  if (m_active_scene) {
    m_active_scene->processButtonInteractions();
  }
}

void SceneManager::resetButtonInteractions() {
  if (m_active_scene) {
    m_active_scene->resetButtonInteractions();
  }
}

std::unordered_set<Button *> SceneManager::getButtonInstances() {
  if (m_active_scene) {
    return m_active_scene->getButtonInstances();
  } else {
    return {};
  }
}
