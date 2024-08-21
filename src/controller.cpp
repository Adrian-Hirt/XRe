#include <xre/controller.h>

Controller::Controller() {
  // Create the model for visualizing the controllers
  m_model = ModelFactory::createCube({0.67f, 0.84f, 0.9f, 1.0f});
  m_root_node = SceneNode();
  m_model_node = SceneNode(&m_model);
  m_model_node.scale(0.03f, 0.03f, 0.075f);

  // Create the model for visualizing intersections of the aim line
  m_aim_indicator_sphere = ModelFactory::createSphere();
  m_aim_indicator_sphere.setColor({0.0f, 0.75f, 1.0f, 1.0f});
  m_intersection_sphere_node = SceneNode(&m_aim_indicator_sphere);
  m_intersection_sphere_node.scale(0.1f, 0.1f, 0.1f);

  m_root_node.addChildNode(m_model_node);
  m_root_node.addChildNode(m_intersection_sphere_node);

  // Create the shaders for the controller
  m_controller_shader = Shader::loadOrCreate(SHADERS_FOLDER "/ambient.hlsl");

  // Create the line for the aim direction
  m_aim_line = Line({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f});
}

void Controller::render() {
  // Return early if the controller is not active
  if (!m_active) {
    return;
  }

  // Color the model a different color depending on the "grab"
  // state of the controller
  if (m_grabbing) {
    m_model.setColor({1.0f, 0.0f, 0.0f, 1.0f});
  }
  else {
    m_model.setColor({0.67f, 0.84f, 0.9f, 1.0f});
  }

  m_root_node.render();

  // Render the aim line
  m_aim_line.render(m_controller_shader);
}

void Controller::updatePosition(DirectX::XMVECTOR current_origin) {
  // Return early if the controller is not active
  if (!m_active) {
    return;
  }

  DirectX::XMVECTOR controller_position = DirectX::XMLoadFloat3((DirectX::XMFLOAT3 *)&m_pose.position);
  DirectX::XMVECTOR controller_orientation = DirectX::XMLoadFloat4((DirectX::XMFLOAT4 *)&m_pose.orientation);

  // Apply the global teleport translation from moving the origin
  controller_position = DirectX::XMVectorAdd(controller_position, current_origin);

  // Set position and orientation of the scene node
  m_model_node.setPosition(controller_position);
  m_model_node.setRotation(controller_orientation);

  // Update the aim line
  m_aim_line.updateAimLineFromControllerPose(controller_position,
                                             DirectX::XMLoadFloat4((DirectX::XMFLOAT4 *)&m_aim.orientation),
                                             current_origin,
                                             Controller::s_line_intersection_threshold);
}

void Controller::computeSceneInteractions() {
  // Nothing to do if the controller is not active
  if(!m_active) {
    return;
  }

  m_root_node.updateTransformation();

  DirectX::BoundingOrientedBox controller_bounding_box = m_model_node.getTransformedBoundingBox();

  // Check if any of our controllers is grabbing a grabbable node
  for(SceneNode *current_node : SceneNode::getGrabbableInstances()) {
    // Skip this if we already are grabbing this node with another controller or a hand
    if (current_node->m_grabbed) {
      continue;
    }

    if(current_node->intersects(controller_bounding_box)) {
      // Keep track that we're intersecting with this model
      current_node->m_intersected_in_current_frame = true;

      // Also, if the controller is grabbing, set the position and the rotation of the
      // model to those of the controller
      if (m_grabbing) {
        current_node->m_grabbed = true;
        current_node->setPosition(m_model_node.getPosition());
        current_node->setRotation(m_model_node.getRotation());
      }
    }
  }
}

std::optional<DirectX::XMVECTOR> Controller::updateIntersectionSphereAndComputePossibleTeleport() {
  if(!m_active) {
    return std::nullopt;
  }

  // Next, check if we need to render the aim intersection sphere
  m_intersection_sphere_node.m_render = false;

  float closest_grabbable_aim_intersection = computeAimIndicatorSpherePosition(SceneNode::getGrabbableInstances());
  float closest_terrain_aim_intersection = computeAimIndicatorSpherePosition(SceneNode::getTerrainInstances());

  if (m_intersection_sphere_node.m_render) {
    // The direction vector has unit length, i.e. to stretch it to the required length, we
    // simple multiply the vector with the length, which gives us a new vector.
    DirectX::XMVECTOR stretched_direction = m_aim_line.getLineDirection() * std::min(closest_grabbable_aim_intersection, closest_terrain_aim_intersection);

    DirectX::XMVECTOR sphere_position = m_aim_line.getLineStart();
    sphere_position = DirectX::XMVectorAdd(sphere_position, stretched_direction);
    m_intersection_sphere_node.setPosition(sphere_position);
    m_aim_indicator_sphere.setColor({0.0f, 0.75f, 1.0f, 1.0f});

    if (closest_terrain_aim_intersection < closest_grabbable_aim_intersection) {
      // Paint the indicator spheres yellow if we can teleport to their location
      m_aim_indicator_sphere.setColor({1.0f, 1.0f, 0.0f, 1.0f});

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
  float closest_intersection_distance = Controller::s_line_intersection_threshold + 1;

  for(SceneNode *current_node : nodes) {
    // Check if the node intersects the line of the controller
    float intersection_distance;

    if(current_node->intersects(m_aim_line.getLineStart(), m_aim_line.getLineDirection(), &intersection_distance)) {
      if (intersection_distance > 0 && intersection_distance <= Controller::s_line_intersection_threshold) {
        m_intersection_sphere_node.m_render = true;

        if (closest_intersection_distance > intersection_distance) {
          closest_intersection_distance = intersection_distance;
        }
      }
    }
  }

  return closest_intersection_distance;
}
