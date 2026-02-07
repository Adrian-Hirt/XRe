#pragma once

// XRe includes
#include <xre/component.h>
#include <xre/input.h>

class InteractionComponent : public Component {
public:
  virtual void onHoverBegin(Input& input) = 0;
  virtual void onHoverEnd(Input& input) = 0;

  virtual void onGrabBegin(Input& input) = 0;
  virtual void onGrabUpdate(Input& input) = 0;
  virtual void onGrabEnd(Input& input) = 0;

  virtual void onTrigger(Input& input) = 0;
};