// Get a component based on type
template <typename T> T *SceneNode::getComponent() {
  for (auto& c : m_components) {
    if (auto* casted = dynamic_cast<T*>(c.get())) {
      return casted;
    }
  }

  // No such component found
  return nullptr;
}

template<typename T> std::vector<T*> SceneNode::getComponents() {
  std::vector<T*> result;
  for (auto& c : m_components) {
    if (auto* casted = dynamic_cast<T*>(c.get())) {
      result.push_back(casted);
    }
  }
  return result;
}