#pragma once

// DirectX includes
#include <d3d11.h>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>

class XreDx11Handler {
public:
  XreDx11Handler();
  XreDx11Handler(LUID &adapter_luid);

  ID3D11Device* get_device();
  ID3D11DeviceContext* get_device_context();
  swapchain_data_t create_render_targets(ID3D11Texture2D &texture);
  void render_frame(XrCompositionLayerProjectionView& view, swapchain_data_t& swapchain_data);

  DXGI_FORMAT d3d11_swapchain_format = DXGI_FORMAT_R8G8B8A8_UNORM;

private:
  ID3D11Device *device;
  ID3D11DeviceContext *device_context;

  // Methods
  bool initialize_device(LUID &adapter_luid);
};
