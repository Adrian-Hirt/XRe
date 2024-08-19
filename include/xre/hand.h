#pragma once

// DirectX includes
#include <d3d11.h>
#include <DirectXMath/DirectXMath.h>

// OpenXR includes
#include <open_xr/openxr.h>

// XRe includes
#include <xre/model.h>
#include <xre/shader.h>
#include <xre/model_factory.h>

class Hand {
public:
  // Constructor
  Hand(XrHandEXT hand_identifier);

  // Render the hand
  void render();

  // Update the position of the controller
  void updatePosition(DirectX::XMVECTOR current_origin);

  // Compute scene interactions of the controllers
  void computeSceneInteractions();


  void updateHandGrabAndPinchState();

  XrHandTrackerEXT m_hand_tracker;
  XrHandJointLocationEXT m_joint_locations[XR_HAND_JOINT_COUNT_EXT];
  XrHandEXT m_hand_identifier;

  bool m_active = false;

private:
  Shader m_joint_shader;
  std::vector<Model> m_joints;

  bool m_pinching = false;

  const static XrSpaceLocationFlags s_pose_valid_flags = XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;

  static inline XrHandJointEXT s_fingertips[4] = {
    XR_HAND_JOINT_INDEX_TIP_EXT,
    XR_HAND_JOINT_MIDDLE_TIP_EXT,
    XR_HAND_JOINT_RING_TIP_EXT,
    XR_HAND_JOINT_LITTLE_TIP_EXT
  };

  bool jointIsFingerTip(int joint_index, bool include_thumb);
};
