#pragma once

// XRe includes
#include <xre/interaction_component.h>

class GrabbbableComponent : public InteractionComponent {
public:
  void onHoverBegin(Input& input) override;
  void onHoverEnd(Input& input) override;

  void onGrabBegin(Input& input) override;
  void onGrabUpdate(Input& input) override;
  void onGrabEnd(Input& input) override;

  void onTrigger(Input& input) override;
};