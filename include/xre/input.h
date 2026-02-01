#pragma once

// XRe includes
#include <xre/scene_node.h>

// Other includes
#include <memory>

class Input {
public:
  virtual ~Input() = default;
  virtual std::shared_ptr<SceneNode> getSceneNodeForSceneNodeUpdate() = 0;
  virtual std::vector<std::shared_ptr<SceneNode>> getSceneNodeForInteractionQuery() = 0;

  virtual inline bool isValid() { return true; };

  // Whether the input is active or not
  bool m_active = false;

  // Boolean for the grab action
  bool m_grabbing = false;

  // Boolean for the pinch action, usually only
  // used for hands
  bool m_pinching = false;
private:

};
