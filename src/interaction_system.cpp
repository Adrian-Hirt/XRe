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
}

void InteractionSystem::queryInteractions() {
  // Compute interactions for controllers
  queryContollerInteractions(m_left_controller_state);
  queryContollerInteractions(m_right_controller_state);

  // Compute interactions for hands
  queryHandInteractions(m_left_hand_state);
  queryHandInteractions(m_right_hand_state);
}

void InteractionSystem::queryContollerInteractions(InputState& state) {
  auto controller = std::get<std::shared_ptr<Controller>>(state.input);

  // Nothing to do if the controller is not active
  if (!controller || !controller->m_active) {
    return;
  }

  // Check if any of our controllers is grabbing a grabbable node
  for (SceneNode *current_node : SceneManager::instance().getGrabbableNodeInstances()) {
    // Skip this if we already are grabbing this node with another controller or a hand
    if (current_node->m_grabbed) {
      continue;
    }

    if (current_node->intersects(controller->m_model_node)) {
      state.hits.push_back(current_node);
    }
  }

  // Check if any of the buttons are activated
  for (Button *button : SceneManager::instance().getButtonInstances()) {
    // Get the scene node of the button
    auto scene_node = button->getSceneNode();

    if (scene_node->intersects(controller->m_model_node)) {
      state.hits.push_back(scene_node.get());
    }
  }
}

void InteractionSystem::queryHandInteractions(InputState& state) {
  auto hand = std::get<std::shared_ptr<Hand>>(state.input);

  // Nothing to do if the hand is not active or invalid pose
  if (!hand || !hand->m_active || !hand->isValid()) {
    return;
  }

  // Check if the hand is intersecting a grabbable node. To make it simpler for the moment, we only
  // check intersection with the palm and the tip of the thumb (as for "grab", both the thumb and the
  // center of the palm should intersect, and for "pinch", the tip of the thumb needs to intersect).
  auto thumb_scene_node = hand->getThumbSceneNode();
  auto palm_scene_node = hand->getThumbSceneNode();

  for (SceneNode *current_node : SceneManager::instance().getGrabbableNodeInstances()) {
    if (current_node->intersects(thumb_scene_node) || current_node->intersects(palm_scene_node)) {
      // Keep track that we're intersecting with this model
      state.hits.push_back(current_node);
    }
  }

  // Check if any of the buttons are activated
  for (Button *button : SceneManager::instance().getButtonInstances()) {
    // Get the scene node of the button
    auto scene_node = button->getSceneNode();

    if (scene_node->intersects(thumb_scene_node) || scene_node->intersects(palm_scene_node)) {
      // Keep track that we're intersecting with this model
      state.hits.push_back(scene_node.get());
    }
  }
}

void InteractionSystem::processInteractions() {
  // First resolve the hits to the input that should act on it
  auto resolved_hits = resolveInteractions();

  // Then for the resolved hits, run the corresponding code
  for (auto [scene_node, input] : resolved_hits) {
    scene_node->m_intersected_in_current_frame = true;

    if (auto controller = std::get_if<std::shared_ptr<Controller>>(&input)) {
      // If the controller is grabbing, set the position and the rotation of the
      // model to those of the controller
      if ((*controller)->m_grabbing) {
        scene_node->m_grabbed = true;
        scene_node->setPosition((*controller)->m_model_node->getPosition());
        scene_node->setRotation((*controller)->m_model_node->getRotation());
      }
    }
    else if (auto hand = std::get_if<std::shared_ptr<Hand>>(&input)) {
      // If the hand is pinching, set the position and rotation of the model to that of the thumb
      if ((*hand)->m_pinching) {
        auto thumb_scene_node = (*hand)->getThumbSceneNode();
        scene_node->m_grabbed = true;
        scene_node->setPosition(thumb_scene_node->getPosition());
        scene_node->setRotation(thumb_scene_node->getRotation());
      }
    }
  }

  // Process button triggers
  SceneManager::instance().processButtonInteractions();
}

std::unordered_map<SceneNode*, Input> InteractionSystem::resolveInteractions() {
  std::unordered_map<SceneNode*, Input> result = {};

  for(InputState& input_state : m_priority_ordered_states) {
    for (auto hit : input_state.hits) {
      if (!result.contains(hit)) {
        result.emplace(hit, input_state.input);
      }
    }
  }

  return result;
}

