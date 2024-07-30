#include <xre/hand.h>

Hand::Hand(XrHandEXT hand_identifier) {
  m_hand_identifier = hand_identifier;
  m_joint_shader = Shader::loadOrCreate(SHADERS_FOLDER "/ambient.hlsl");

  // This could probably be replaced by instancing, where we only have one cube but draw it multiple
  // times. For the first version we'll keep this approach though.
  for (int i = 0; i < XR_HAND_JOINT_COUNT_EXT; i++) {
    Model model = ModelFactory::createCube({0.67f, 0.84f, 0.9f, 1.0f});
    model.setScale(0.005f, 0.005f, 0.005f);
    m_joints.push_back(model);
  }
}

void Hand::render(DirectX::XMVECTOR current_origin) {
  const XrSpaceLocationFlags pose_valid_flags = XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;

  for (int i = 0; i < XR_HAND_JOINT_COUNT_EXT; i++) {
    // Only render the joint if its pose is valid
    if ((m_joint_locations[i].locationFlags & pose_valid_flags) != pose_valid_flags) {
      continue;
    }

    Model current_model = m_joints[i];
    XrPosef pose = m_joint_locations[i].pose;

    DirectX::XMVECTOR joint_position = DirectX::XMLoadFloat3((DirectX::XMFLOAT3 *)&pose.position);
    DirectX::XMVECTOR joint_orientation = DirectX::XMLoadFloat4((DirectX::XMFLOAT4 *)&pose.orientation);
    float joint_scale =  m_joint_locations[i].radius;

    // Apply the global teleport translation from moving the origin
    joint_position = DirectX::XMVectorAdd(joint_position, current_origin);

    // Update the cube model
    current_model.setPosition(joint_position);
    current_model.setRotation(joint_orientation);
    current_model.setScale(joint_scale, joint_scale, joint_scale);

    // And render the model
    current_model.render(m_joint_shader);
  }
}

