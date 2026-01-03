#pragma once

// Other includes
#include <optional>
#include <memory>

// OpenXR includes
#include <open_xr/openxr.h>

// XRe includes
#include <xre/model.h>
#include <xre/model_factory.h>
#include <xre/line.h>
#include <xre/scene_node.h>
#include <xre/structs.h>
#include <xre/material.h>
#include <xre/vulkan_handler.h>
#include <xre/button.h>
#include <xre/scene_manager.h>

class Controller {
public:
  // Constructor
  Controller(std::shared_ptr<Material> material, std::shared_ptr<VulkanHandler> vulkan_handler);

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
  static constexpr float s_line_intersection_far_threshold = 6.0f;
  static constexpr float s_line_intersection_near_threshold = 0.1f;

  // Model for the controller
  std::shared_ptr<Model> m_model;

  // Line for visualizing the aim direction
  std::shared_ptr<Line> m_aim_line;

  // Lenght of the line to render
  float m_aim_line_render_length;

  // Model to visualize intersections of the aim line
  // and models marked as interactable
  std::shared_ptr<Model> m_aim_indicator_sphere;

  // Scene nodes
  SceneNode m_root_node;
  std::shared_ptr<SceneNode> m_model_node;
  std::shared_ptr<SceneNode> m_intersection_sphere_node;
  std::shared_ptr<SceneNode> m_aim_line_node;

  float computeAimIndicatorSpherePosition(std::unordered_set<SceneNode *> models);
};
