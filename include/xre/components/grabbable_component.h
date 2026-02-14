#pragma once

// XRe includes
#include <xre/components/interaction_component.h>

class GrabbableComponent : public InteractionComponent {
public:
  void onGrabBegin(Input &input) override;
  void onGrabUpdate(Input &input) override;
  void onGrabEnd(Input &input) override;

private:
  Input *m_grabbing_input = nullptr;
};