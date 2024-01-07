#include <xre/controller.h>

Controller::Controller() {
  model = ModelFactory::createCube({0.67f, 0.84f, 0.9f, 1.0f});
  model.scale(0.03f, 0.03f, 0.075f);

  controller_shader = Shader(SHADERS_FOLDER "/ambient.hlsl");
}

void Controller::render() {
  DirectX::XMVECTOR controller_position = DirectX::XMLoadFloat3((DirectX::XMFLOAT3 *)&pose.position);
  DirectX::XMVECTOR controller_orientation = DirectX::XMLoadFloat4((DirectX::XMFLOAT4 *)&pose.orientation);

  // Set position and orientation of the model
  model.setPosition(controller_position);
  model.setRotation(controller_orientation);

  model.render(&controller_shader);
}
