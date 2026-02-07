#pragma once

// XRe includes
#include <xre/interaction_component.h>

// Other includes
#include <functional>

class ButtonComponent : public InteractionComponent {
public:
  ButtonComponent() = delete;
  ButtonComponent(std::function<void()> trigger_callback);

  void onHoverBegin(Input &input) override;
  void onHoverEnd(Input &input) override;

  // Grabbing is not doing anything
  void onGrabBegin(Input &input) override {};
  void onGrabUpdate(Input &input) override {};
  void onGrabEnd(Input &input) override {};

private:
  // Callback when button is triggered
  std::function<void()> m_trigger_callback;
};