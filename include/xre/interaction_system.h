#pragma once

// XRe includes
#include <xre/scene_manager.h>
#include <xre/controller.h>
#include <xre/hand.h>
#include <xre/scene_node.h>
#include <xre/components/component.h>

// Other includes
#include <memory>
#include <vector>
#include <variant>

struct InputState {
  std::shared_ptr<Input> input;

  // Nodes hit in the current frame
  std::vector<SceneNode *> hits;

  // Node that was hovered / grabbed last frame
  SceneNode *last_hovered_node = nullptr;
  SceneNode *last_grabbed_node = nullptr;

  // Teleport location requested by a given input
  std::optional<glm::vec3> requested_teleport_location;
};

class InteractionSystem {
public:
  InteractionSystem(std::shared_ptr<Controller> left_controller, std::shared_ptr<Controller> right_controller,
                    std::shared_ptr<Hand> left_hand, std::shared_ptr<Hand> right_hand);

  void beginFrame();
  void queryInteractions();
  void processInteractions();
  std::optional<glm::vec3> getTeleportLocation();

private:
  // Controllers
  InputState m_left_controller_state;
  InputState m_right_controller_state;

  // Hands
  InputState m_left_hand_state;
  InputState m_right_hand_state;

  // Threshold for showing the line intersection point
  static constexpr float LINE_INTERSECTION_FAR_THRESHOLD = 6.0f;
  static constexpr float LINE_INTERSECTION_NEAR_THRESHOLD = 0.1f;

  // States ordered by priority
  std::vector<std::reference_wrapper<InputState>> m_priority_ordered_states;

  void queryInputInteractions(InputState &state);

  std::unordered_map<SceneNode *, std::shared_ptr<Input>> resolveInteractions();

  template <typename T>
    requires std::derived_from<T, InteractionComponent>
  void processInteractionNodes(const std::vector<T *> &components, InputState &state);
  void processAimLineInteractions(InputState &state);
  std::optional<float> computeAimIndicatorSphereDistance(std::unordered_set<SceneNode *> nodes, std::shared_ptr<Line> aim_line);
};

#include <xre/interaction_system.inl>
