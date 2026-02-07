#include <xre/interaction_system.h>

InteractionSystem::InteractionSystem(std::shared_ptr<Controller> left_controller, std::shared_ptr<Controller> right_controller,
                                     std::shared_ptr<Hand> left_hand, std::shared_ptr<Hand> right_hand) {
  // Setup the ordering by priority, with highest prio being at front.
  // Higher number takes precedence. We let hands take precedence over controllers,
  // and right over left.
  m_priority_ordered_states = {};

  // Only add the hands if present
  if (right_hand) {
    m_right_hand_state.input = right_hand;
    m_priority_ordered_states.push_back(m_right_hand_state);
  }

  if (left_hand) {
    m_left_hand_state.input = left_hand;
    m_priority_ordered_states.push_back(m_left_hand_state);
  }

  if (right_controller) {
    m_right_controller_state.input = right_controller;
    m_priority_ordered_states.push_back(m_right_controller_state);
  }

  if (left_controller) {
    m_left_controller_state.input = left_controller;
    m_priority_ordered_states.push_back(m_left_controller_state);
  }
}

void InteractionSystem::beginFrame() {
  // Reset the interaction tracking booleans on the grabbable SceneNodes
  SceneManager::instance().resetInteractionStates();
  SceneManager::instance().resetButtonInteractions();

  // Reset the hits
  for (InputState &input_state : m_priority_ordered_states) {
    input_state.hits = {};
  }
}

void InteractionSystem::queryInteractions() {
  // Compute interactions for controllers
  queryInputInteractions(m_left_controller_state);
  queryInputInteractions(m_right_controller_state);

  // Compute interactions for hands
  queryInputInteractions(m_left_hand_state);
  queryInputInteractions(m_right_hand_state);
}

void InteractionSystem::queryInputInteractions(InputState &state) {
  // Nothing to do if the input is not active or invalid
  if (!state.input || !state.input->m_active || !state.input->isValid()) {
    return;
  }

  for (auto component : SceneManager::instance().getGrabbableComponents()) {
    auto scene_node = component->getSceneNode();
    for (auto input_node_to_check : state.input->getSceneNodeForInteractionQuery()) {
      if (scene_node->intersects(input_node_to_check)) {
        // Keep track that we're intersecting with this model
        state.hits.push_back(scene_node);
        break;
      }
    }
  }

  // Check if any of the buttons are activated
  for (Button *button : SceneManager::instance().getButtonInstances()) {
    // Get the scene node of the button
    auto scene_node = button->getSceneNode();

    for (auto input_node_to_check : state.input->getSceneNodeForInteractionQuery()) {
      if (scene_node->intersects(input_node_to_check)) {
        // Keep track that we're intersecting with this model
        state.hits.push_back(scene_node.get());
        break;
      }
    }
  }
}

void InteractionSystem::processInteractions() {
  // First resolve the hits to the input that should act on it
  auto resolved_hits = resolveInteractions();

  // Then for the resolved hits, run the corresponding code
  for (auto [scene_node, input] : resolved_hits) {
    for (auto *component : scene_node->getComponents<InteractionComponent>()) {
      component->onHoverBegin(*input);

      if (input->m_grabbing || input->m_pinching) {
        component->onGrabUpdate(*input);
      }
    }
  }

  // // Process button triggers
  // SceneManager::instance().processButtonInteractions();
}

std::unordered_map<SceneNode *, std::shared_ptr<Input>> InteractionSystem::resolveInteractions() {
  std::unordered_map<SceneNode *, std::shared_ptr<Input>> result = {};

  for (InputState &input_state : m_priority_ordered_states) {
    for (auto hit : input_state.hits) {
      if (!result.contains(hit)) {
        result.emplace(hit, input_state.input);
      }
    }
  }

  return result;
}
