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

void Controller::render() {
  // Return early if the controller should not be rendered alltogether
  if (!m_should_render) {
    return;
  }

  DirectX::XMVECTOR controller_position = DirectX::XMLoadFloat3((DirectX::XMFLOAT3 *)&m_pose.position);
  DirectX::XMVECTOR controller_orientation = DirectX::XMLoadFloat4((DirectX::XMFLOAT4 *)&m_pose.orientation);

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

  m_model.render(&m_controller_shader);

  // Update the aim line and render it
  m_aim_line.updateLineFromXrPose(m_aim, Controller::s_line_intersection_threshold);
  m_aim_line.render();

  // Render the aim interaction sphere
  if (m_render_intersection_sphere) {
    m_aim_indicator_sphere.render(&m_aim_indicator_shader);
  }
}

void Controller::sceneModelInteractions() {
  if(!m_should_render) {
    return;
  }

  DirectX::BoundingOrientedBox controller_bounding_box = m_model.getTransformedBoundingBox();

  m_render_intersection_sphere = false;

  for(Model *current_model : Model::getGrabbableInstances()) {
    // TODO: maybe set a bit a better indicator that an object is intersecting, e.g. a glow effect
    if(current_model->intersects(controller_bounding_box)) {
      // Set a different color if the controller is intersecting another model
      current_model->setColor({1.0f, 0.0f, 0.0f, 1.0f});

      // Also, if the controller is grabbing, set the position and the rotation of the
      // model to those of the controller
      if (m_grabbing) {
        current_model->setPosition(m_model.getPosition());
        current_model->setRotation(m_model.getRotation());
      }
    }
    else {
      current_model->resetColor();
    }

    // Check if the model intersects the line of the controller
    float intersection_distance;

    if(current_model->intersects(m_aim_line.getLineStart(), m_aim_line.getLineDirection(), &intersection_distance)) {
      if (intersection_distance > 0 && intersection_distance <= Controller::s_line_intersection_threshold) {
        m_render_intersection_sphere = true;

        // The direction vector has unit length, i.e. to stretch it to the required length, we
        // simple multiply the vector with the length, which gives us a new vector.
        DirectX::XMVECTOR stretched_direction = m_aim_line.getLineDirection() * intersection_distance;

        DirectX::XMVECTOR sphere_position = m_aim_line.getLineStart();
        sphere_position = DirectX::XMVectorAdd(sphere_position, stretched_direction);

        DirectX::XMFLOAT3 v3F;
        DirectX::XMStoreFloat3(&v3F, sphere_position);

        m_aim_indicator_sphere.setPosition(sphere_position);
      }
    }
  }
}
