#pragma once

// OpenXR includes
#include <open_xr/openxr.h>

// XRe includes
#include <xre/model.h>
#include <xre/model_factory.h>
#include <xre/scene_node.h>
#include <xre/material.h>

class Hand {
public:
  // Constructor
  Hand(XrHandEXT hand_identifier, Material material);

  // Render the hand
  void render(RenderContext &ctx);

  // Update the position of the hand
  void updatePosition(glm::vec3 current_origin);

  // Compute scene interactions of the hand
  void computeSceneInteractions();

  void updateHandGrabAndPinchState();

  XrHandTrackerEXT m_hand_tracker;
  XrHandJointLocationEXT m_joint_locations[XR_HAND_JOINT_COUNT_EXT];
  XrHandEXT m_hand_identifier;

  bool m_active = false;

private:
  SceneNode *m_hand_root_node;
  std::vector<SceneNode *> m_joint_nodes;

  bool m_pinching = false;

  const static XrSpaceLocationFlags s_pose_valid_flags = XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;

  // clang-format off
  static inline XrHandJointEXT s_fingertips[4] = {
    XR_HAND_JOINT_INDEX_TIP_EXT,
    XR_HAND_JOINT_MIDDLE_TIP_EXT,
    XR_HAND_JOINT_RING_TIP_EXT,
    XR_HAND_JOINT_LITTLE_TIP_EXT
  };
  // clang-format on

  bool jointIsFingerTip(int joint_index, bool include_thumb);
};
