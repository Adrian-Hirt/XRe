#pragma once

// XRe includes
#include <xre/component.h>
#include <xre/input.h>

class InteractionComponent : public Component {
public:
  virtual void onHoverBegin(Input &input) { m_hovered = true; }
  virtual void onHoverEnd(Input &input) { m_hovered = false; }

  virtual void onGrabBegin(Input &input) = 0;
  virtual void onGrabUpdate(Input &input) = 0;
  virtual void onGrabEnd(Input &input) = 0;

  bool isHovered() { return m_hovered; }
private:
  bool m_hovered = false;
};