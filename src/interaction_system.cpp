#include <xre/interaction_system.h>

InteractionSystem::InteractionSystem(std::shared_ptr<Controller> left_controller, std::shared_ptr<Controller> right_controller,
                                     std::shared_ptr<Hand> left_hand, std::shared_ptr<Hand> right_hand) {
  m_left_controller = left_controller;
  m_right_controller = right_controller;
  m_left_hand = left_hand;
  m_right_hand = right_hand;
}

void InteractionSystem::beginFrame() {
  // Reset the interaction tracking booleans on the grabbable SceneNodes
  SceneManager::instance().resetInteractionStates();
  SceneManager::instance().resetButtonInteractions();
}

void InteractionSystem::queryInteractions() {
  // Compute interactions for controllers
  queryContollerInteractions(m_left_controller);
  queryContollerInteractions(m_right_controller);

  // Compute interactions for hands
  queryHandInteractions(m_left_hand);
  queryHandInteractions(m_right_hand);
}

void InteractionSystem::queryContollerInteractions(std::shared_ptr<Controller> controller) {
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
      // Keep track that we're intersecting with this model
      current_node->m_intersected_in_current_frame = true;

      // Also, if the controller is grabbing, set the position and the rotation of the
      // model to those of the controller
      if (controller->m_grabbing) {
        current_node->m_grabbed = true;
        current_node->setPosition(controller->m_model_node->getPosition());
        current_node->setRotation(controller->m_model_node->getRotation());
      }
    }
  }

  // Check if any of the buttons are activated
  for (Button *button : SceneManager::instance().getButtonInstances()) {
    // Get the scene node of the button
    auto scene_node = button->getSceneNode();

    // Skip if the other controller already intersects
    if (scene_node->m_intersected_in_current_frame) {
      continue;
    }

    if (scene_node->intersects(controller->m_model_node)) {
      // Keep track that we're intersecting with this model
      scene_node->m_intersected_in_current_frame = true;
    }
  }
}

void InteractionSystem::queryHandInteractions(std::shared_ptr<Hand> hand) {
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
    // Skip this if we already are grabbing this node with another controller or a hand
    if (current_node->m_grabbed) {
      continue;
    }

    if (current_node->intersects(thumb_scene_node) || current_node->intersects(palm_scene_node)) {
      // Keep track that we're intersecting with this model
      current_node->m_intersected_in_current_frame = true;

      // Also, if the hand is pinching, set the position and rotation of the model to that of the thumb
      if (hand->m_pinching) {
        current_node->m_grabbed = true;
        current_node->setPosition(thumb_scene_node->getPosition());
        current_node->setRotation(thumb_scene_node->getRotation());
      }
    }
  }

  // TODO: If the hand is closed, we need to check for intersection with the central
  // palm joint position, to determine whether the hand is grabbing something.
}

void InteractionSystem::processInteractions() {
  // Process button triggers
  SceneManager::instance().processButtonInteractions();
}

