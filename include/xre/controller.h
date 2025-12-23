#pragma once

// // DirectX includes
// #include <DirectXMath/DirectXMath.h>

// Other includes
#include <optional>

// OpenXR includes
#include <open_xr/openxr.h>

// XRe includes
#include <xre/model.h>
#include <xre/model_factory.h>
#include <xre/line.h>
#include <xre/scene_node.h>
#include <xre/structs.h>

class Controller {
public:
  // Constructor
  Controller();

  // Render the controller
  void render(RenderContext &ctx);

  // Check whether we want to teleport
  std::optional<glm::vec3> updateIntersectionSphereAndComputePossibleTeleport();

  // Update the position of the controller
  void updatePosition(glm::vec3 current_origin);

  // Compute scene interactions of the controllers
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

  // Model for the controller
  Model m_model;

  // Line for visualizing the aim direction
  Line m_aim_line;

  // Model to visualize intersections of the aim line
  // and models marked as interactable
  Model m_aim_indicator_sphere;

  // Scene nodes
  SceneNode m_root_node;
  SceneNode m_model_node;
  SceneNode m_intersection_sphere_node;
  SceneNode m_aim_line_node;

  float computeAimIndicatorSpherePosition(std::unordered_set<SceneNode *> models);
};
