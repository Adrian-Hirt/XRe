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
    auto resolvedHits = resolveInteractions();

    for (InputState &state : m_priority_ordered_states) {
      SceneNode* hovered_node = nullptr;
      SceneNode* grabbed_node = nullptr;

      // Determine which node this input will act on (priority first)
      // TODO: make deterministic, such that we always pick the same node
      for (auto hit : state.hits) {
        hovered_node = hit;

        // If input is grabbing/pinching, this is the grabbed node
        if (state.input->m_grabbing || state.input->m_pinching) {
          grabbed_node = hit;
        }
        break; // stop at first hit
      }

      // Hover phase, if the new hovered node is not the
      // previous hovered node
      if (hovered_node != state.last_hovered_node) {
        // End hover on previous
        if (state.last_hovered_node) {
          for (auto* component : state.last_hovered_node->getComponents<InteractionComponent>()) {
            component->onHoverEnd(*state.input);
          }
        }
        // Begin hover on new node
        if (hovered_node) {
          for (auto* component : hovered_node->getComponents<InteractionComponent>()) {
            component->onHoverBegin(*state.input);
          }
        }
      }

      // Grab phase, if the new grabbed node is not the
      // previous grabbed node
      if (grabbed_node != state.last_grabbed_node) {
        // End grab on previous
        if (state.last_grabbed_node) {
          for (auto* component : state.last_grabbed_node->getComponents<InteractionComponent>()) {
            component->onGrabEnd(*state.input);
          }
        }
        // Begin grab on new node
        if (grabbed_node) {
          for (auto* component : grabbed_node->getComponents<InteractionComponent>()) {
            component->onGrabBegin(*state.input);
          }
        }
      }

      // Always run update grab on current node
      if (grabbed_node) {
        for (auto* component : grabbed_node->getComponents<InteractionComponent>()) {
          component->onGrabUpdate(*state.input);
        }
      }

      // Save for next frame
      state.last_hovered_node = hovered_node;
      state.last_grabbed_node = grabbed_node;
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
