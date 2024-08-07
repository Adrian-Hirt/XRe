#include <xre/controller.h>

Controller::Controller() {
  // Create the model for visualizing the controllers
  m_model = ModelFactory::createCube({0.67f, 0.84f, 0.9f, 1.0f});
  m_model.scale(0.03f, 0.03f, 0.075f);

  // Create the model for visualizing intersections of the aim line
  m_aim_indicator_sphere = ModelFactory::createSphere(0.1f);
  m_aim_indicator_sphere.setColor({0.0f, 0.75f, 1.0f, 1.0f});

  // Create the shaders for the controller
  m_controller_shader = Shader::loadOrCreate(SHADERS_FOLDER "/ambient.hlsl");
  m_aim_indicator_shader = Shader::loadOrCreate(SHADERS_FOLDER "/color.hlsl");

  // Create the line for the aim direction
  m_aim_line = Line({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f});
}

void Controller::render(DirectX::XMVECTOR current_origin) {
  // Return early if the controller is not active
  if (!m_active) {
    return;
  }

  DirectX::XMVECTOR controller_position = DirectX::XMLoadFloat3((DirectX::XMFLOAT3 *)&m_pose.position);
  DirectX::XMVECTOR controller_orientation = DirectX::XMLoadFloat4((DirectX::XMFLOAT4 *)&m_pose.orientation);

  // Apply the global teleport translation from moving the origin
  controller_position = DirectX::XMVectorAdd(controller_position, current_origin);

  // Set position and orientation of the model
  m_model.setPosition(controller_position);
  m_model.setRotation(controller_orientation);

  // Color the model a different color depending on the "grab"
  // state of the controller
  if (m_grabbing) {
    m_model.setColor({1.0f, 0.0f, 0.0f, 1.0f});
  }
  else {
    m_model.setColor({0.67f, 0.84f, 0.9f, 1.0f});
  }

  m_model.render(m_controller_shader);

  // Update the aim line and render it
  m_aim_line.updateAimLineFromControllerPose(controller_position, DirectX::XMLoadFloat4((DirectX::XMFLOAT4 *)&m_aim.orientation), current_origin, Controller::s_line_intersection_threshold);
  m_aim_line.render(m_controller_shader);

  // Render the aim interaction sphere
  if (m_render_intersection_sphere) {
    m_aim_indicator_sphere.render(m_aim_indicator_shader);
  }
}

std::optional<DirectX::XMVECTOR> Controller::sceneModelInteractions() {
  if(!m_active) {
    return std::nullopt;
  }

  DirectX::BoundingOrientedBox controller_bounding_box = m_model.getTransformedBoundingBox();

  // Check if any of our controllers is grabbing a grabbable model
  for(Model *current_model : Model::getGrabbableInstances()) {
    current_model->setGrabbed(false);

    // TODO: maybe set a bit a better indicator that an object is intersecting, e.g. a glow effect
    if(current_model->intersects(controller_bounding_box)) {
      // Set a different color if the controller is intersecting another model
      current_model->setColor({1.0f, 0.0f, 0.0f, 1.0f});

      // Also, if the controller is grabbing, set the position and the rotation of the
      // model to those of the controller
      if (m_grabbing) {
        current_model->setGrabbed(true);
        current_model->setPosition(m_model.getPosition());
        current_model->setRotation(m_model.getRotation());
      }
    }
    else {
      current_model->resetColor();
    }
  }

  // Next, check if we need to render the aim intersection sphere
  m_render_intersection_sphere = false;

  float closest_grabbable_aim_intersection = computeAimIndicatorSpherePosition(Model::getGrabbableInstances());
  float closest_terrain_aim_intersection = computeAimIndicatorSpherePosition(Model::getTerrainInstances());

  if (m_render_intersection_sphere) {
    // The direction vector has unit length, i.e. to stretch it to the required length, we
    // simple multiply the vector with the length, which gives us a new vector.
    DirectX::XMVECTOR stretched_direction = m_aim_line.getLineDirection() * std::min(closest_grabbable_aim_intersection, closest_terrain_aim_intersection);

    DirectX::XMVECTOR sphere_position = m_aim_line.getLineStart();
    sphere_position = DirectX::XMVectorAdd(sphere_position, stretched_direction);
    m_aim_indicator_sphere.setPosition(sphere_position);
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

float Controller::computeAimIndicatorSpherePosition(std::unordered_set<Model *> models) {
  // As we only want to highlight the intersection with the closest model,
  // we need to keep track of the smallest threshold. We probably should replace
  // this later with sorting the elements by distance from the camera and then check in
  // ascending distance, but for now this will have to do.
  float closest_intersection_distance = Controller::s_line_intersection_threshold + 1;

  for(Model *current_model : models) {
    // Check if the model intersects the line of the controller
    float intersection_distance;

    if(current_model->intersects(m_aim_line.getLineStart(), m_aim_line.getLineDirection(), &intersection_distance)) {
      if (intersection_distance > 0 && intersection_distance <= Controller::s_line_intersection_threshold) {
        m_render_intersection_sphere = true;

        if (closest_intersection_distance > intersection_distance) {
          closest_intersection_distance = intersection_distance;
        }
      }
    }
  }

  return closest_intersection_distance;
}
