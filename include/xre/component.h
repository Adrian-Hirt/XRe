#pragma once

// Forward definition
class SceneNode;

class Component {
public:
  virtual ~Component() = default;

  virtual void onAttach(SceneNode& node) { m_node = &node; }
  virtual void onDetach() {}

  SceneNode* getSceneNode() const { return m_node; }
protected:
  SceneNode* m_node = nullptr;
};
