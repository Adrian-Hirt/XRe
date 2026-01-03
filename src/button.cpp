#include <xre/button.h>

Button::Button(Scene* scene, std::shared_ptr<Material> material, bool disable_on_trigger, std::function<void()> trigger_callback, std::shared_ptr<VulkanHandler> vulkan_handler) {
  // Setup the model and the scene nodes
  m_model = ModelFactory::createCube({1.0f, 0.0f, 0.0f}, material, vulkan_handler);
  m_root_node = std::make_shared<SceneNode>(m_model);
  m_root_node->scale(0.1f, 0.1f, 0.02f);

  // Store boolean
  m_disable_on_trigger = disable_on_trigger;

  // Store the callbacks
  m_trigger_callback = trigger_callback;

  // Store button in list of buttons in scene
  scene->addButton(this);
}

std::shared_ptr<SceneNode> Button::getSceneNode() {
  return m_root_node;
}

void Button::trigger() {
  // Disable button if needed
  if (m_disable_on_trigger) {
    m_enabled = false;
  }

  m_trigger_callback();
}

void Button::processInteractions() {
  // Skip disabled buttons
  if (!m_enabled) {
    return;
  }

  if (m_root_node->m_intersected_in_current_frame && !m_root_node->m_was_intersected_in_previous_frame) {
    trigger();
  }

  m_root_node->m_was_intersected_in_previous_frame = m_root_node->m_intersected_in_current_frame;
}

void Button::resetInteractionState() {
  m_root_node->m_intersected_in_current_frame = false;
}

bool Button::isEnabled() {
  return m_enabled;
}
