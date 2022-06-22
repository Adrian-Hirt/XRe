#pragma once

// Defines for OpenXR
#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11

// DirectX includes
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

// OpenXR includes
#include <open_xr/openxr.h>
#include <open_xr/openxr_platform.h>

// XRe includes
#include <xre/utils.h>
#include <xre/xre_dx11_handler.h>

// Other includes
#include <iostream>
#include <vector>

class XrOpenXrHandler {
public:
  XrOpenXrHandler();
  XrOpenXrHandler(const char *application_name);
  ~XrOpenXrHandler();

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

  // Pointers to ext functions we need to use
  PFN_xrGetD3D11GraphicsRequirementsKHR ext_xrGetD3D11GraphicsRequirementsKHR;

  // Handlers
  XreDx11Handler xr_dx11_handler;

  // Methods
  bool initialize_open_xr();
};
