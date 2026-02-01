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

struct InteractionHit {
  SceneNode* node;
  float distance;
};

struct InputState {
  std::variant<std::shared_ptr<Controller>, std::shared_ptr<Hand>> input;
  std::vector<InteractionHit> hits;
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

  void queryContollerInteractions(InputState state);
  void queryHandInteractions(InputState state);
};
