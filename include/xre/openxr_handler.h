#pragma once

// Defines for OpenXR
#define XR_USE_GRAPHICS_API_VULKAN

// Vulkan includes
#include <vulkan/vulkan.h>

// OpenXR includes
#include <open_xr/openxr.h>
#include <open_xr/openxr_platform.h>

// GLM includes
#include <glm/glm/vec3.hpp>

// XRe includes
#include <xre/utils.h>
#include <xre/geometry.h>
#include <xre/structs.h>
#include <xre/vulkan_handler.h>
#include <xre/render_target.h>
#include <xre/controller.h>
#include <xre/hand.h>
#include <xre/material.h>

// Other includes
#include <iostream>
#include <vector>
#include <functional>

class OpenXrHandler {
public:
  OpenXrHandler();
  OpenXrHandler(const char *application_name);
  ~OpenXrHandler();
  VkExtent2D getEyeResolution(size_t eyeIndex) const;
  void pollOpenxrEvents(bool &loop_running, bool &xr_running);
  void renderFrame(std::function<void(RenderContext &)> draw_callback, std::function<void(XrTime)> update_simulation_callback);
  void renderLayer(XrTime predicted_time, XrCompositionLayerProjection &layer_projection,
                   std::function<void(RenderContext &)> draw_callback);

  // Handlers
  VulkanHandler m_vulkan_handler;

private:
  // Configs
  const char *m_application_name;
  XrFormFactor m_application_form_factor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY; // Using a HMD
  XrViewConfigurationType m_application_view_type =
      XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; // The HMD has a stereo view, one display for each eye

  // OpenXR variables we'll need through the application
  XrInstance m_openxr_instance;                      // OpenXR instance
  XrSystemId m_openxr_system_id = XR_NULL_SYSTEM_ID; // The ID of the OpenXR system
  XrSession m_openxr_session;
  XrEnvironmentBlendMode m_openxr_blend_mode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE; // Blend mode (opaque / transparent) to use
  XrSpace m_openxr_stage_space;
  XrSpace m_openxr_view_space;
  std::vector<XrView> m_openxr_views;
  std::vector<XrViewConfigurationView> m_openxr_view_configuration_views;

  // System properties
  XrSystemProperties m_openxr_system_properties = {XR_TYPE_SYSTEM_PROPERTIES};
  XrSystemHandTrackingPropertiesEXT m_openxr_hand_tracking_system_properties = {XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT};

  // New:
  std::vector<XrCompositionLayerProjectionView> m_projection_views;
  std::vector<XrSwapchain> m_swapchains;
  std::vector<std::vector<RenderTarget *>> m_render_targets;
  XrSessionState m_openxr_session_state = XR_SESSION_STATE_UNKNOWN;
  uint32_t m_view_count = 0u;
  std::vector<glm::mat4> m_view_matrices;
  std::vector<glm::mat4> m_projection_matrices;

  // // Pointers to ext functions we need to use
  PFN_xrCreateHandTrackerEXT m_ext_xrCreateHandTrackerEXT;
  PFN_xrDestroyHandTrackerEXT m_ext_xrDestroyHandTrackerEXT;
  PFN_xrLocateHandJointsEXT m_ext_xrLocateHandJointsEXT;

  // Controllers
  Controller *m_left_controller = NULL;
  Controller *m_right_controller = NULL;

  // Hands
  Hand *m_left_hand = NULL;
  Hand *m_right_hand = NULL;

  // Actions
  XrActionSet m_default_action_set;
  XrAction m_controller_pose_action;
  XrAction m_controller_aim_action;
  XrAction m_controller_grab_action;
  XrAction m_controller_teleport_action;

  // For checking if the pose of a controller is valid
  const static XrSpaceLocationFlags s_pose_valid_flags = XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;

  glm::vec3 m_headset_position = glm::zero<glm::vec3>();
  glm::vec3 m_current_origin = glm::zero<glm::vec3>();

  // Methods
  bool initializeOpenxr();
  void initializeOpenxrActions();
  void initializeHandTracking();
  void setupActionBindings();
  void suggestBindings(std::string interaction_profile, std::vector<XrActionSuggestedBinding> bindings);
  void pollOpenxrActions(XrTime predicted_time);
  void updateControllerStates(Controller *controller, XrTime predicted_time);
  void renderInteractions(RenderContext &ctx);
  void updateHandTrackingStates(Hand *hand, XrTime predicted_time);
  void updateCurrentOriginForTeleport(glm::vec3 teleport_location);
  XrPath getXrPathFromString(std::string string);
};
