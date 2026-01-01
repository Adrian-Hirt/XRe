#include <xre/button.h>

Button::Button(std::shared_ptr<Material> material, bool disable_on_trigger, std::function<void()> trigger_callback, std::shared_ptr<VulkanHandler> vulkan_handler) {
  // Setup the model and the scene nodes
  m_model = ModelFactory::createCube({1.0f, 0.0f, 0.0f}, material, vulkan_handler);
  m_root_node = std::make_shared<SceneNode>(m_model);
  m_root_node->scale(0.1f, 0.1f, 0.02f);

  // Store boolean
  m_disable_on_trigger = disable_on_trigger;

  // Store the callbacks
  m_trigger_callback = trigger_callback;

  // Store instance
  s_instances.insert(this);
}

std::shared_ptr<SceneNode> Button::getRootNode() {
  return m_root_node;
}

void Button::trigger() {
  // Disable button if needed
  if (m_disable_on_trigger) {
    m_enabled = false;
  }

  m_trigger_callback();
}

std::unordered_set<Button *> Button::getInstances() {
  return s_instances;
}

void Button::processButtonTriggers() {
  for(auto button : s_instances) {
    // Skip disabled buttons
    if (!button->m_enabled) {
      continue;
    }

    auto scene_node = button->getRootNode();

    if (scene_node->m_intersected_in_current_frame && !scene_node->m_was_intersected_in_previous_frame) {
      button->trigger();
    }

    scene_node->m_was_intersected_in_previous_frame = scene_node->m_intersected_in_current_frame;
  }
}

void Button::resetInteractionStates() {
  for (auto button : s_instances) {
    auto scene_node = button->getRootNode();
    scene_node->m_intersected_in_current_frame = false;
  }
}
