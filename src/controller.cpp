#include <xre/controller.h>

Controller::Controller() {
  // Create the model for visualizing the controllers
  model = ModelFactory::createCube({0.67f, 0.84f, 0.9f, 1.0f});
  model.scale(0.03f, 0.03f, 0.075f);

  // Create the shader for the controller models
  controller_shader = Shader(SHADERS_FOLDER "/ambient.hlsl");

  // Create the line for the aim direction
  aim_line = Line({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f});
}

void Controller::render() {
  // Return early if the controller should not be rendered alltogether
  if (!shouldRender) {
    return;
  }

  DirectX::XMVECTOR controller_position = DirectX::XMLoadFloat3((DirectX::XMFLOAT3 *)&pose.position);
  DirectX::XMVECTOR controller_orientation = DirectX::XMLoadFloat4((DirectX::XMFLOAT4 *)&pose.orientation);

  // Set position and orientation of the model
  model.setPosition(controller_position);
  model.setRotation(controller_orientation);

  // Color the model a different color depending on the "grab"
  // state of the controller
  if (grabbing) {
    model.setColor({1.0f, 0.0f, 0.0f, 1.0f});
  }
  else {
    model.setColor({0.67f, 0.84f, 0.9f, 1.0f});
  }

  model.render(&controller_shader);

  // Update the aim line and render it
  aim_line.updateLineFromXrPose(aim);
  aim_line.render();
}

void Controller::sceneModelInteractions() {
  if(!shouldRender) {
    return;
  }

  DirectX::BoundingOrientedBox controller_bounding_box = model.getTransformedBoundingBox();

  for(Model *current_model : Model::interactable_instances) {
    // TODO: maybe set a bit a better indicator that an object is intersecting, e.g. a glow effect
    if(current_model->intersects(controller_bounding_box)) {
      // Set a different color if the controller is intersecting another model
      current_model->setColor({1.0f, 0.0f, 0.0f, 1.0f});

      // Also, if the controller is grabbing, set the position and the rotation of the
      // model to those of the controller
      if (grabbing) {
        current_model->setPosition(model.getPosition());
        current_model->setRotation(model.getRotation());
      }
    }
    else {
      current_model->resetColor();
    }
  }
}
