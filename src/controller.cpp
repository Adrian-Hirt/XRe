#include <xre/controller.h>

Controller::Controller(std::shared_ptr<Material> material, std::shared_ptr<VulkanHandler> vulkan_handler) {
  // Create the model for visualizing the controllers
  m_model = ModelFactory::createCube({0.67f, 0.84f, 0.9f}, material, vulkan_handler);
  m_root_node = SceneNode();
  m_model_node = std::make_shared<SceneNode>(m_model);
  m_model_node->scale(0.03f, 0.03f, 0.075f);

  // Create the model for visualizing intersections of the aim line
  m_aim_indicator_sphere = ModelFactory::createSphere(material, {0.0f, 0.75f, 1.0f}, vulkan_handler);
  m_intersection_sphere_node = std::make_shared<SceneNode>(m_aim_indicator_sphere);
  m_intersection_sphere_node->scale(0.05f, 0.05f, 0.05f);

  m_root_node.addChildNode(m_model_node);
  m_root_node.addChildNode(m_intersection_sphere_node);

  // Create the line for the aim direction
  m_aim_line = std::make_shared<Line>(0.003f, 2.0f, glm::vec3(1.0f, 0.0f, 0.0f), material, vulkan_handler);
}

void Controller::render(RenderContext &ctx) {
  // Return early if the controller is not active
  if (!m_active) {
    return;
  }

  // Color the model a different color depending on the "grab"
  // state of the controller
  if (m_grabbing) {
    m_model->setColor({1.0f, 0.0f, 0.0f});
  } else {
    m_model->resetColor();
  }

  m_root_node.render(ctx);
  m_aim_line->render(ctx);
}

void Controller::updatePosition(glm::vec3 current_origin) {
  // Return early if the controller is not active
  if (!m_active) {
    return;
  }

  glm::vec3 controller_position = Utils::toVec3(m_pose.position);
  glm::quat controller_orientation = Utils::toQuat(m_pose.orientation);

  // Apply the global teleport translation from moving the origin
  controller_position = controller_position + current_origin;

  // Set position and orientation of the scene node
  m_model_node->setPosition(controller_position);
  m_model_node->setRotation(controller_orientation);

  // Update the aim line
  m_aim_line->updateAimLineFromControllerPose(controller_position, Utils::toQuat(m_aim.orientation), m_aim_line_render_length);
}

void Controller::computeSceneInteractions() {
  // Nothing to do if the controller is not active
  if (!m_active) {
    return;
  }

  m_root_node.updateTransformation();

  // Check if any of our controllers is grabbing a grabbable node
  for (SceneNode *current_node : SceneManager::instance().getGrabbableNodeInstances()) {
    // Skip this if we already are grabbing this node with another controller or a hand
    if (current_node->m_grabbed) {
      continue;
    }

    if (current_node->intersects(m_model_node)) {
      // Keep track that we're intersecting with this model
      current_node->m_intersected_in_current_frame = true;

      // Also, if the controller is grabbing, set the position and the rotation of the
      // model to those of the controller
      if (m_grabbing) {
        current_node->m_grabbed = true;
        current_node->setPosition(m_model_node->getPosition());
        current_node->setRotation(m_model_node->getRotation());
      }
    }
  }

  // Check if any of the buttons are activated
  for (Button *button : SceneManager::instance().getButtonInstances()) {
    // Get the scene node of the button
    auto scene_node = button->getRootNode();
    
    // Skip if the other controller already intersects
    if (scene_node->m_intersected_in_current_frame) {
      continue;
    }

    if (scene_node->intersects(m_model_node)) {
      // Keep track that we're intersecting with this model
      scene_node->m_intersected_in_current_frame = true;
    }
  }
}

std::optional<glm::vec3> Controller::updateIntersectionSphereAndComputePossibleTeleport() {
  if (!m_active) {
    return std::nullopt;
  }

  // Reset state of the intersection sphere
  m_intersection_sphere_node->setActive(false);

  // Reset line length
  m_aim_line_render_length = LINE_INTERSECTION_FAR_THRESHOLD;

  float closest_grabbable_aim_intersection = computeAimIndicatorSpherePosition(SceneManager::instance().getGrabbableNodeInstances());
  float closest_terrain_aim_intersection = computeAimIndicatorSpherePosition(SceneManager::instance().getTerrainInstances());

  if (m_intersection_sphere_node->isActive()) {
    float closest_intersection = std::min(closest_grabbable_aim_intersection, closest_terrain_aim_intersection);

    // Set length of line to render, which needs to be the half of the closest intersection (due to how
    // we render the line).
    m_aim_line_render_length = closest_intersection / 2.0f;

    // The direction vector has unit length, i.e. to stretch it to the required length, we
    // simple multiply the vector with the length, which gives us a new vector.
    glm::vec3 stretched_direction = m_aim_line->getLineDirection() * closest_intersection;

    glm::vec3 sphere_position = m_aim_line->getLineStart();
    sphere_position = sphere_position + stretched_direction;
    m_intersection_sphere_node->setPosition(sphere_position);
    m_aim_indicator_sphere->setColor({0.0f, 0.75f, 1.0f});

    if (closest_terrain_aim_intersection < closest_grabbable_aim_intersection) {
      // Paint the indicator spheres yellow if we can teleport to their location
      m_aim_indicator_sphere->setColor({1.0f, 1.0f, 0.0f});

      // If a teleporting is requested and there is an intersection sphere rendered, we can
      // check whether the target is a terrain, and if yes, teleport to that location.
      if (m_teleporting_requested) {
        return sphere_position;
      }
    }
  }

  return std::nullopt;
}

float Controller::computeAimIndicatorSpherePosition(std::unordered_set<SceneNode *> nodes) {
  // As we only want to highlight the intersection with the closest model,
  // we need to keep track of the smallest threshold. We probably should replace
  // this later with sorting the elements by distance from the camera and then check in
  // ascending distance, but for now this will have to do.
  float closest_intersection_distance = LINE_INTERSECTION_FAR_THRESHOLD + 1;

  for (SceneNode *current_node : nodes) {
    // Check if the node intersects the line of the controller
    float intersection_distance;
    glm::vec3 start = m_aim_line->getLineStart();
    glm::vec3 dir = m_aim_line->getLineDirection();

    // if(current_node->intersects(m_aim_line.getLineStart(), m_aim_line.getLineDirection(), &intersection_distance)) {
    if (current_node->intersects(start, dir, &intersection_distance)) {
      if (intersection_distance > 0 && intersection_distance <= LINE_INTERSECTION_FAR_THRESHOLD &&
          intersection_distance >=LINE_INTERSECTION_NEAR_THRESHOLD) {
        m_intersection_sphere_node->setActive(true);

        if (closest_intersection_distance > intersection_distance) {
          closest_intersection_distance = intersection_distance;
        }
      }
    }
  }

  return closest_intersection_distance;
}
