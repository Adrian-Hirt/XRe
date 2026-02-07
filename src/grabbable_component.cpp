#include <xre/grabbable_component.h>

void GrabbbableComponent::onHoverBegin(Input& input) {
  m_node->m_intersected_in_current_frame = true;
}

void GrabbbableComponent::onHoverEnd(Input& input) {

}

void GrabbbableComponent::onGrabBegin(Input& input) {

}

void GrabbbableComponent::onGrabUpdate(Input& input) {
  m_node->m_grabbed = true;
  m_node->setPosition(input.getSceneNodeForSceneNodeUpdate()->getPosition());
  m_node->setRotation(input.getSceneNodeForSceneNodeUpdate()->getRotation());
}

void GrabbbableComponent::onGrabEnd(Input& input) {

}

void GrabbbableComponent::onTrigger(Input& input) {

}
