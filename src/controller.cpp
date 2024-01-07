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
  DirectX::XMVECTOR controller_position = DirectX::XMLoadFloat3((DirectX::XMFLOAT3 *)&pose.position);
  DirectX::XMVECTOR controller_orientation = DirectX::XMLoadFloat4((DirectX::XMFLOAT4 *)&pose.orientation);

  // Set position and orientation of the model
  model.setPosition(controller_position);
  model.setRotation(controller_orientation);

  model.render(&controller_shader);

  // Update the aim line and render it
  aim_line.updateLineFromXrPose(aim);
  aim_line.render();
}