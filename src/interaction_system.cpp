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
  // Reset the hits as well as the requested teleport location
  for (InputState &input_state : m_priority_ordered_states) {
    input_state.hits = {};
    input_state.requested_teleport_location = std::nullopt;
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

  // Process the components
  processInteractionNodes(SceneManager::instance().getGrabbableComponents(), state);
  processInteractionNodes(SceneManager::instance().getButtonComponents(), state);

  // Process the "aim line" interactions
  if (state.input->hasAimLine()) {
    processAimLineInteractions(state);
  }
}

void InteractionSystem::processAimLineInteractions(InputState &state) {
  if (!state.input->m_active) {
    // Teleport location for this input should not set, can return early
    return;
  }

  std::unordered_set<SceneNode *> grabbable_scene_nodes;
  for (auto *comp : SceneManager::instance().getGrabbableComponents()) {
    grabbable_scene_nodes.insert(comp->getSceneNode());
  }

  std::unordered_set<SceneNode *> teleportable_scene_nodes;
  for (auto *comp : SceneManager::instance().getTeleportTargetComponents()) {
    teleportable_scene_nodes.insert(comp->getSceneNode());
  }

  auto aim_line = state.input->getAimLine();
  auto closest_grabbable_aim_intersection = computeAimIndicatorSphereDistance(grabbable_scene_nodes, aim_line);
  auto closest_terrain_aim_intersection = computeAimIndicatorSphereDistance(teleportable_scene_nodes, aim_line);

  // If we have either a closest intersection with some grabbable or terrain instances,
  // we need to continue.
  if (closest_grabbable_aim_intersection.has_value() || closest_terrain_aim_intersection.has_value()) {
    // Get the closest value
    float closest_intersection;

    if (closest_grabbable_aim_intersection.has_value() && closest_terrain_aim_intersection.has_value()) {
      closest_intersection = std::min(closest_grabbable_aim_intersection.value(), closest_terrain_aim_intersection.value());
    } else if (closest_grabbable_aim_intersection.has_value()) {
      closest_intersection = closest_grabbable_aim_intersection.value();
    } else {
      closest_intersection = closest_terrain_aim_intersection.value();
    }

    // Set length of line to render, which needs to be the half of the closest intersection (due to how
    // we render the line).
    float aim_line_render_length = closest_intersection / 2.0f;

    // The direction vector has unit length, i.e. to stretch it to the required length, we
    // simple multiply the vector with the length, which gives us a new vector.
    glm::vec3 stretched_direction = aim_line->getLineDirection() * closest_intersection;

    glm::vec3 sphere_position = aim_line->getLineStart();
    sphere_position = sphere_position + stretched_direction;

    // if the closest terrain aim intersection is set, and either there is no closer grabbable intersection
    // or the closest grabbable intersection is farther away, we can trigger a teleport.
    bool terrain_intersection = closest_terrain_aim_intersection.has_value() &&
                                (!closest_grabbable_aim_intersection.has_value() ||
                                 closest_terrain_aim_intersection.value() < closest_grabbable_aim_intersection.value());

    // If a teleporting is requested and there is an intersection sphere rendered, we can
    // check whether the target is a terrain, and if yes, teleport to that location.
    if (terrain_intersection && state.input->m_teleporting_requested) {
      state.requested_teleport_location = sphere_position;
    }

    state.input->updateAimIndicators(aim_line_render_length, sphere_position, terrain_intersection);
    return;
  }

  state.input->updateAimIndicators(LINE_INTERSECTION_FAR_THRESHOLD, std::nullopt, false);
}

std::optional<float> InteractionSystem::computeAimIndicatorSphereDistance(std::unordered_set<SceneNode *> nodes,
                                                                          std::shared_ptr<Line> aim_line) {
  // As we only want to highlight the intersection with the closest model,
  // we need to keep track of the smallest threshold. We probably should replace
  // this later with sorting the elements by distance from the camera and then check in
  // ascending distance, but for now this will have to do.
  std::optional<float> closest_intersection_distance;
  glm::vec3 start = aim_line->getLineStart();
  glm::vec3 dir = aim_line->getLineDirection();

  for (SceneNode *current_node : nodes) {
    // Get the collider component of the node
    auto collider_component = current_node->getComponent<ColliderComponent>();
    if (collider_component == nullptr) {
      continue;
    }

    // Check if the node intersects the line of the controller
    float intersection_distance = 0.0f;

    // Skip to next collider if we do not have an intersection
    if (!collider_component->intersects(start, dir, &intersection_distance)) {
      continue;
    }

    // Check if the intersection is between the two thresholds we set, if not, we can skip
    // to the next iteration.
    if (intersection_distance < LINE_INTERSECTION_NEAR_THRESHOLD || intersection_distance > LINE_INTERSECTION_FAR_THRESHOLD) {
      continue;
    }

    if (!closest_intersection_distance || intersection_distance < *closest_intersection_distance) {
      closest_intersection_distance = intersection_distance;
    }
  }

  return closest_intersection_distance;
}

void InteractionSystem::processInteractions() {
  auto resolvedHits = resolveInteractions();

  for (InputState &state : m_priority_ordered_states) {
    SceneNode *hovered_node = nullptr;
    SceneNode *grabbed_node = nullptr;

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
        for (auto *component : state.last_hovered_node->getComponents<InteractionComponent>()) {
          component->onHoverEnd(*state.input);
        }
      }
      // Begin hover on new node
      if (hovered_node) {
        for (auto *component : hovered_node->getComponents<InteractionComponent>()) {
          component->onHoverBegin(*state.input);
        }
      }
    }

    // Grab phase, if the new grabbed node is not the
    // previous grabbed node
    if (grabbed_node != state.last_grabbed_node) {
      // End grab on previous
      if (state.last_grabbed_node) {
        for (auto *component : state.last_grabbed_node->getComponents<InteractionComponent>()) {
          component->onGrabEnd(*state.input);
        }
      }
      // Begin grab on new node
      if (grabbed_node) {
        for (auto *component : grabbed_node->getComponents<InteractionComponent>()) {
          component->onGrabBegin(*state.input);
        }
      }
    }

    // Always run update grab on current node
    if (grabbed_node) {
      for (auto *component : grabbed_node->getComponents<InteractionComponent>()) {
        component->onGrabUpdate(*state.input);
      }
    }

    // Save for next frame
    state.last_hovered_node = hovered_node;
    state.last_grabbed_node = grabbed_node;
  }
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

std::optional<glm::vec3> InteractionSystem::getTeleportLocation() {
  // If any of the ordered input states has a teleport location requested,
  // return it. Otherwise, return nullopt.
  for (InputState &input_state : m_priority_ordered_states) {
    if (input_state.requested_teleport_location.has_value()) {
      return input_state.requested_teleport_location.value();
    }
  }

  return std::nullopt;
}
