template <typename T>
requires std::derived_from<T, InteractionComponent>
void InteractionSystem::processInteractionNodes(const std::vector<T*>& components, InputState &state) {
  for (T* component : components) {
    // Get the collider component on this node. We need to add the template keyword
    // to get C++ to know that the getComponent method is a template, as this method
    // here is also in a template.
    auto scene_node = component->getSceneNode();
    auto collider_component = scene_node->template getComponent<ColliderComponent>();

    // And return early if there is no such component
    if (!collider_component) {
      return;
    }

    // Otherwise, compute the intersection of the colliders the input
    // gives us (usually one for a controller, multiple for hands).
    for (auto input_node_to_check : state.input->getSceneNodeForInteractionQuery()) {
      auto input_node_collider_component = input_node_to_check->template getComponent<ColliderComponent>();
      // Get the input node collider component
      if (!input_node_collider_component) {
        return;
      }

      if (input_node_collider_component->intersects(collider_component)) {
        // Keep track that we're intersecting with this model
        state.hits.push_back(scene_node);
        break;
      }
    }
  }
}
