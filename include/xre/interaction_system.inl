template <typename T>
requires std::derived_from<T, InteractionComponent>
void InteractionSystem::processInteractionNodes(const std::vector<T*>& components, InputState &state) {
  for (T* component : components) {
    auto scene_node = component->getSceneNode();
    for (auto input_node_to_check : state.input->getSceneNodeForInteractionQuery()) {
      if (scene_node->intersects(input_node_to_check)) {
        // Keep track that we're intersecting with this model
        state.hits.push_back(scene_node);
        break;
      }
    }
  }
}
