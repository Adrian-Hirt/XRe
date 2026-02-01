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
  m_left_controller->computeSceneInteractions();
  m_right_controller->computeSceneInteractions();

  // Compute interactions for hands
  if (m_left_hand && m_right_hand) {
    m_left_hand->computeSceneInteractions();
    m_right_hand->computeSceneInteractions();
  }
}

void InteractionSystem::processInteractions() {
  // Process button triggers
  SceneManager::instance().processButtonInteractions();
}

