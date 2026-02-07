#include <xre/button_component.h>

ButtonComponent::ButtonComponent(std::function<void()> trigger_callback) {
  m_trigger_callback = trigger_callback;
}

void ButtonComponent::onHoverBegin(Input &input) {
  InteractionComponent::onHoverBegin(input);
  m_trigger_callback();
}

void ButtonComponent::onHoverEnd(Input &input) {
  InteractionComponent::onHoverEnd(input);
}