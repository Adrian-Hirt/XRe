#pragma once

// Other includes
#include <optional>

// OpenXR includes
#include <open_xr/openxr.h>

// XRe includes
#include <xre/model.h>
#include <xre/shader.h>
#include <xre/model_factory.h>
#include <xre/line.h>

class Controller {
public:
  // Constructor
  Controller();

  // render a controller
  void render(DirectX::XMVECTOR current_origin);

  // Check whether we want to teleport
  std::optional<DirectX::XMVECTOR> updateIntersectionSphereAndComputePossibleTeleport();

  // Check whether the controller interacts with any models
  void computeSceneInteractions();

  // Whether the controller is active or not
  bool m_active = false;

  // Path for the controller, e.g. `/user/hand/left`
  XrPath m_controller_path;

  // Space for the pose action
  XrSpace m_pose_space;

  // Space for the aim action
  XrSpace m_aim_space;

  // Boolean for the grab action
  bool m_grabbing = false;

  // Space for the teleport action
  bool m_teleporting_requested = false;

  // Pose of the controller
  XrPosef m_pose;

  // Aim of the controller
  XrPosef m_aim;

private:
  // Threshold for showing the line intersection point
  static constexpr float s_line_intersection_threshold = 6.0f;

  // Model and shader for the controller
  Model m_model;
  Shader m_controller_shader;

  // Line for visualizing the aim direction
  Line m_aim_line;

  // Model to visualize intersections of the aim line
  // and models marked as interactable
  Model m_aim_indicator_sphere;
  Shader m_aim_indicator_shader;

  bool m_render_intersection_sphere = false;

  float computeAimIndicatorSpherePosition(std::unordered_set<Model *> models);
};
