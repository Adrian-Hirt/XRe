#pragma once

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
  void render();

  // Check whether the controller interacts with any models
  void sceneModelInteractions();

  // Whether the controller should be rendered or not
  bool m_should_render;

  // Path for the controller, e.g. `/user/hand/left`
  XrPath m_controller_path;

  // Path and space for the pose action
  XrPath m_pose_path;
  XrSpace m_pose_space;

  // Path and space for the aim action
  XrPath m_aim_path;
  XrSpace m_aim_space;

  // Path and space for the grab action
  XrPath m_grab_path;
  bool m_grabbing;

  // Pose of the controller
  XrPosef m_pose;

  // Aim of the controller
  XrPosef m_aim;

private:
  // Model and shader for the controller
  Model m_model;
  Shader m_controller_shader;

  // Line for visualizing the aim direction
  Line m_aim_line;

  // Threshold for showing the line intersection point
  static constexpr float s_line_intersection_threshold = 4.0f;
};
