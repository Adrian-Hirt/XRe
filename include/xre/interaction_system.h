#pragma once

// XRe includes
#include <xre/scene_manager.h>
#include <xre/controller.h>
#include <xre/hand.h>
#include <xre/scene_node.h>

// Other includes
#include <memory>
#include <vector>
#include <variant>

using Input = std::variant<std::shared_ptr<Controller>, std::shared_ptr<Hand>>;

struct InputState {
  Input input;
  std::vector<SceneNode*> hits;
  int priority;
};

class InteractionSystem {
public:
  InteractionSystem(std::shared_ptr<Controller> left_controller, std::shared_ptr<Controller> right_controller,
                    std::shared_ptr<Hand> left_hand, std::shared_ptr<Hand> right_hand);

  void beginFrame();
  void queryInteractions();
  void processInteractions();

private:
  // Controllers
  InputState m_left_controller_state;
  InputState m_right_controller_state;

  // Hands
  InputState m_left_hand_state;
  InputState m_right_hand_state;

  // States ordered by priority
  std::vector<std::reference_wrapper<InputState>> m_priority_ordered_states;

  void queryContollerInteractions(InputState& state);
  void queryHandInteractions(InputState& state);

  std::unordered_map<SceneNode*, Input> resolveInteractions();
};
