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

  // Path for the controller, e.g. `/user/hand/left`
  XrPath controller_path;

  // Path and space for the pose action
  XrPath pose_path;
  XrSpace pose_space;

  // Path and space for the aim action
  XrPath aim_path;
  XrSpace aim_space;

  // Keep track wether the controller is active or not
  bool active;

  Shader controller_shader;

  // Pose of the controller
  XrPosef pose;

  // Aim of the controller
  XrPosef aim;

private:
  // Model for the controller
  Model model;

  // Line for visualizing the aim direction
  Line aim_line;
};
