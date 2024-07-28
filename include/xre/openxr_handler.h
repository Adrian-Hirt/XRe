#pragma once

// Defines for OpenXR
#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11

// DirectX includes
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath/DirectXMath.h>

// OpenXR includes
#include <open_xr/openxr.h>
#include <open_xr/openxr_platform.h>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/dx11_handler.h>
#include <xre/controller.h>

// Other includes
#include <iostream>
#include <vector>
#include <functional>

class OpenXrHandler {
public:
  OpenXrHandler();
  OpenXrHandler(const char *application_name);
  ~OpenXrHandler();
  void pollOpenxrEvents(bool &loop_running, bool &xr_running);
  void renderFrame(std::function<void()> draw_callback, std::function<void(XrTime)> update_simulation_callback);
  void renderLayer(XrTime predicted_time,
                    std::vector<XrCompositionLayerProjectionView>& views,
                    XrCompositionLayerProjection& layer_projection,
                    std::function<void()> draw_callback);

  ID3D11Device* getDevice();
  ID3D11DeviceContext* getDeviceContext();

  // Handlers
  Dx11Handler m_dx11_handler;

private:
  // Configs
  const char *m_application_name;
  XrFormFactor m_application_form_factor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY; // Using a HMD
  XrViewConfigurationType m_application_view_type = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; // The HMD has a stereo view, one display for each eye

  // OpenXR variables we'll need through the application
  XrInstance m_openxr_instance; // OpenXR instance
  XrSystemId m_openxr_system_id = XR_NULL_SYSTEM_ID; // The ID of the OpenXR system
  XrSession m_openxr_session;
  XrEnvironmentBlendMode m_openxr_blend_mode; // Blend mode (opaque / transparent) to use
  XrSpace m_openxr_stage_space;
  XrSpace m_openxr_view_space;
  std::vector<XrView> m_openxr_views;
  std::vector<XrViewConfigurationView> m_openxr_view_configuration_views;

  // Swapchains
  std::vector<swapchain_t> m_swapchains;

  // Pointers to ext functions we need to use
  PFN_xrGetD3D11GraphicsRequirementsKHR m_ext_xrGetD3D11GraphicsRequirementsKHR;

  // Controllers
  Controller *m_left_controller = NULL;
  Controller *m_right_controller = NULL;

  // Actions
  XrActionSet m_default_action_set;
  XrAction m_controller_pose_action;
  XrAction m_controller_aim_action;
  XrAction m_controller_grab_action;
  XrAction m_controller_teleport_action;

  // For checking if the pose of a controller is valid
  const static XrSpaceLocationFlags s_pose_valid_flags = XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;

  DirectX::XMVECTOR m_headset_position = { 0.0f, 0.0f, 0.0f };
  DirectX::XMVECTOR m_current_origin = { 0.0f, 0.0f, 0.0f };

  // Methods
  bool initializeOpenxr();
  void initializeOpenxrActions();
  void suggestBindings();
  void pollOpenxrActions(XrTime predicted_time);
  void updateControllerStates(Controller *controller, XrTime predicted_time);
  void updateCurrentOriginForTeleport(DirectX::XMVECTOR teleport_location);
  XrPath getXrPathFromString(std::string string);
};
