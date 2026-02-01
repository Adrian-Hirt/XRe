#include <xre/interaction_system.h>

void InteractionSystem::beginFrame() {
  // Reset the interaction tracking booleans on the grabbable SceneNodes
  SceneManager::instance().resetInteractionStates();
  SceneManager::instance().resetButtonInteractions();
}
