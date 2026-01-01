#include <xre/button.h>

Button::Button(std::shared_ptr<Material> material, std::function<void()> activated_callback, std::shared_ptr<VulkanHandler> vulkan_handler) {
  // Setup the model and the scene nodes
  m_model = ModelFactory::createCube({1.0f, 0.0f, 0.0f}, material, vulkan_handler);
  m_root_node = std::make_shared<SceneNode>(m_model);
  m_root_node->scale(0.1f, 0.1f, 0.02f);


  // Store the callbacks
  m_activated_callback = activated_callback;

  // Store instance
  s_instances.insert(this);
}

std::shared_ptr<SceneNode> Button::getRootNode() {
  return m_root_node;
}

void Button::activate() {
  m_activated_callback();
}

std::unordered_set<Button *> Button::getInstances() {
  return s_instances;
}

void Button::processButtonTriggers() {
  for(auto button : s_instances) {
    auto scene_node = button->getRootNode();

    if (scene_node->m_intersected_in_current_frame && !scene_node->m_was_intersected_in_previous_frame) {
      button->activate();
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
