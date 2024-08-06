#pragma once

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

  void render(DirectX::XMVECTOR current_origin);

  XrHandTrackerEXT m_hand_tracker;
  XrHandJointLocationEXT m_joint_locations[XR_HAND_JOINT_COUNT_EXT];
  XrHandEXT m_hand_identifier;

private:
  Shader m_joint_shader;
  std::vector<Model> m_joints;

  bool jointIsFingerTip(int joint_index, bool include_thumb);
};
