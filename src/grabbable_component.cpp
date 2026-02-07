#include <xre/grabbable_component.h>

void GrabbableComponent::onHoverBegin(Input &input) {
  m_node->m_intersected_in_current_frame = true;
}

void GrabbableComponent::onHoverEnd(Input &input) {
  m_node->m_intersected_in_current_frame = false;
}

void GrabbableComponent::onGrabBegin(Input &input) {
  m_grabbing_input = &input;
}

void GrabbableComponent::onGrabUpdate(Input &input) {
  if (m_grabbing_input != &input) {
    return; // only the grab owner moves it
  }
  m_node->setPosition(input.getSceneNodeForSceneNodeUpdate()->getPosition());
  m_node->setRotation(input.getSceneNodeForSceneNodeUpdate()->getRotation());
}

void GrabbableComponent::onGrabEnd(Input &input) {
  if (m_grabbing_input != &input) {
    return;
  }
  m_grabbing_input = nullptr;
}

void GrabbableComponent::onTrigger(Input &input) {}
