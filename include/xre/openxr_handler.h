#pragma once

// Defines for OpenXR
#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11

// DirectX includes
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath/directxmath.h>

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
  Dx11Handler dx11_handler;

private:
  // Configs
  const char *application_name;
  XrFormFactor application_form_factor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY; // Using a HMD
  XrViewConfigurationType application_view_type = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO; // The HMD has a stereo view, one display for each eye

  // OpenXR variables we'll need through the application
  XrInstance openxr_instance; // OpenXR instance
  XrSystemId openxr_system_id = XR_NULL_SYSTEM_ID; // The ID of the OpenXR system
  XrSession openxr_session;
  XrEnvironmentBlendMode openxr_blend_mode; // Blend mode (opaque / transparent) to use
  XrSpace openxr_space;
  std::vector<XrView> openxr_views;
  std::vector<XrViewConfigurationView> openxr_view_configuration_views;

  // Swapchains
  std::vector<swapchain_t> swapchains;

  // Pointers to ext functions we need to use
  PFN_xrGetD3D11GraphicsRequirementsKHR ext_xrGetD3D11GraphicsRequirementsKHR;

  // Controllers
  Controller *left_controller = NULL;
  Controller *right_controller = NULL;

  // Actions
  XrActionSet default_action_set;
  XrAction controller_pose_action;

  // Methods
  bool initializeOpenxr();
  void initializeOpenxrActions();
  void pollOpenxrActions(XrTime predicted_time);
  void updateControllerStates(Controller *controller, XrTime predicted_time);
};
