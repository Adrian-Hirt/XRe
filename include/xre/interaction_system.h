#pragma once

// XRe includes
#include <xre/scene_manager.h>
#include <xre/controller.h>
#include <xre/hand.h>

// Other includes
#include <memory>

class InteractionSystem {
public:
  InteractionSystem(std::shared_ptr<Controller> left_controller, std::shared_ptr<Controller> right_controller,
                    std::shared_ptr<Hand> left_hand, std::shared_ptr<Hand> right_hand);

  void beginFrame();
  void queryInteractions();
  void processInteractions();

private:
  // Controllers
  std::shared_ptr<Controller> m_left_controller = NULL;
  std::shared_ptr<Controller> m_right_controller = NULL;

  // Hands
  std::shared_ptr<Hand> m_left_hand = NULL;
  std::shared_ptr<Hand> m_right_hand = NULL;
};
