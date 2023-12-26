#pragma once

// DirectX includes
#include <d3d11.h>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/shader.h>
#include <xre/geometry.h>

// Other includes
#include <functional>

class Dx11Handler {
public:
  Dx11Handler();
  Dx11Handler(LUID &adapter_luid);

  ID3D11Device* getDevice();
  ID3D11DeviceContext* getDeviceContext();
  swapchain_data_t createRenderTargets(ID3D11Texture2D &texture);
  void renderFrame(XrCompositionLayerProjectionView& view, swapchain_data_t& swapchain_data, std::function<void(XrCompositionLayerProjectionView&)> draw_callback);

  DXGI_FORMAT d3d11_swapchain_format = DXGI_FORMAT_R8G8B8A8_UNORM;

  void useDefaultRasterizer(bool use_clockwise);
  void useWireframeRasterizer();

private:
  // Device and device context from DirectX 11
  ID3D11Device *device;
  ID3D11DeviceContext *device_context;

  // Device states
  ID3D11RasterizerState *p_rasterizer_state_default_cw;
  ID3D11RasterizerState *p_rasterizer_state_default_ccw;
  ID3D11RasterizerState *p_rasterizer_state_wireframe;
  ID3D11SamplerState *p_sampler_state;
  ID3D11BlendState *p_blend_state;

  // Methods
  void initializeDevice(LUID &adapter_luid);
  void initializeDeviceStates();
};
