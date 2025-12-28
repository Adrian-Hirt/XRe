#include <xre/hand.h>

Hand::Hand(XrHandEXT hand_identifier, Material* material) {
  m_hand_identifier = hand_identifier;
  m_hand_root_node = new SceneNode();

  // This could probably be replaced by instancing, where we only have one cube but draw it multiple
  // times. For the first version we'll keep this approach though.
  for (int i = 0; i < XR_HAND_JOINT_COUNT_EXT; i++) {
    // TODO: use a more sensible approach for lifetime handling
    Model *joint_model = ModelFactory::createCubePtr({0.67f, 0.84f, 0.9}, material);
    SceneNode *joint_node = new SceneNode(joint_model);
    joint_node->setScale(0.005f, 0.005f, 0.005f);
    m_hand_root_node->addChildNode(joint_node);
    m_joint_nodes.push_back(joint_node);
  }
}

bool Hand::jointIsFingerTip(int joint_index, bool include_thumb) {
  // See https://registry.khronos.org/OpenXR/specs/1.0/man/html/XrHandJointEXT.html,
  // The tips of the 5 fingers (including thumb) are the indices:
  //   * 5 => thumb
  //   * 10 => index finger
  //   * 15 => middle finger
  //   * 20 => ring finger
  //   * 25 => little finger
  if (joint_index == 0 || joint_index % 5 != 0) {
    return false;
  }

  // If we include the thumb, we can directly return true,
  // otherwise we can only return true if the index is not
  // equal to 5.
  return include_thumb || joint_index != 5;
}

void Hand::render(RenderContext &ctx) {
  // Nothing to do if the hand is not active
  if (!m_active) {
    return;
  }

  m_hand_root_node->render(ctx);
}

void Hand::updateHandGrabAndPinchState() {
  // Nothing to do if the hand is not active
  if (!m_active) {
    return;
  }

  // Nothing to do if we can't track the thumb
  if ((m_joint_locations[XR_HAND_JOINT_THUMB_TIP_EXT].locationFlags & s_pose_valid_flags) != s_pose_valid_flags) {
    return;
  }

  // Reset pinching state
  m_pinching = false;

  // Compute whether the hand is pinching. For now, "pinching" means at least
  // the tip of one finger is within a small threshold of the tip of the thumb.
  glm::vec3 thumb_position = Utils::toVec3(m_joint_locations[XR_HAND_JOINT_THUMB_TIP_EXT].pose.position);

  const float pinch_threshold = 0.05f; // 5 cm

  for (XrHandJointEXT fingertip : s_fingertips) {
    glm::vec3 tip_position = Utils::toVec3(m_joint_locations[fingertip].pose.position);
    float tip_distance = glm::length(thumb_position - tip_position);

    // Interactions with pinching (e.g. to move an object) should
    // probably happen based on the position of the thumb.
    if (tip_distance < pinch_threshold) {
      m_pinching = true;
    }
  }

  // TODO: Track whether the hand is closed (approach for pinching above might
  // incorrectly report closing hand as pinching).
}

void Hand::updatePosition(glm::vec3 current_origin) {
  // Nothing to do if the hand is not active
  if (!m_active) {
    return;
  }

  for (int i = 0; i < XR_HAND_JOINT_COUNT_EXT; i++) {
    // Only update the joint position its pose is valid
    if ((m_joint_locations[i].locationFlags & s_pose_valid_flags) != s_pose_valid_flags) {
      continue;
    }

    SceneNode *current_node = m_joint_nodes[i];
    XrPosef pose = m_joint_locations[i].pose;

    glm::vec3 joint_position = Utils::toVec3(pose.position);
    glm::quat joint_orientation = Utils::toQuat(pose.orientation);
    float joint_scale = m_joint_locations[i].radius;

    // Apply the global teleport translation from moving the origin
    joint_position = joint_position + current_origin;

    // Update the cube model
    current_node->setPosition(joint_position);
    current_node->setRotation(joint_orientation);
    current_node->setScale(joint_scale, joint_scale, joint_scale);
  }

  m_hand_root_node->updateTransformation();
}

void Hand::computeSceneInteractions() {
  // Nothing to do if the hand is not active
  if (!m_active) {
    return;
  }

  // Nothing to do if either the thumb or the palm is not valid
  bool thumb_valid = (m_joint_locations[XR_HAND_JOINT_THUMB_TIP_EXT].locationFlags & s_pose_valid_flags) == s_pose_valid_flags;
  bool palm_valid = (m_joint_locations[XR_HAND_JOINT_PALM_EXT].locationFlags & s_pose_valid_flags) == s_pose_valid_flags;

  if (!thumb_valid || !palm_valid) {
    return;
  }

  // Check if the hand is intersecting a grabbable node. To make it simpler for the moment, we only
  // check intersection with the palm and the tip of the thumb (as for "grab", both the thumb and the
  // center of the palm should intersect, and for "pinch", the tip of the thumb needs to intersect).
  SceneNode *thumb_scene_node = m_joint_nodes[XR_HAND_JOINT_THUMB_TIP_EXT];
  SceneNode *palm_scene_node = m_joint_nodes[XR_HAND_JOINT_PALM_EXT];

  for (SceneNode *current_node : SceneNode::getGrabbableInstances()) {
    // Skip this if we already are grabbing this node with another controller or a hand
    if (current_node->m_grabbed) {
      continue;
    }

    if (current_node->intersects(*thumb_scene_node) || current_node->intersects(*palm_scene_node)) {
      // Keep track that we're intersecting with this model
      current_node->m_intersected_in_current_frame = true;

      // Also, if the hand is pinching, set the position and rotation of the model to that of the thumb
      if (m_pinching) {
        current_node->m_grabbed = true;
        current_node->setPosition(m_joint_nodes[XR_HAND_JOINT_THUMB_TIP_EXT]->getPosition());
        current_node->setRotation(m_joint_nodes[XR_HAND_JOINT_THUMB_TIP_EXT]->getRotation());
      }
    }
  }

  // TODO: If the hand is closed, we need to check for intersection with the central
  // palm joint position, to determine whether the hand is grabbing something.
}
