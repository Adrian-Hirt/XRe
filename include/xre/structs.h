#pragma once

// DirectX includes
#include <d3d11.h>
#include <directxmath.h>

// OpenXR includes
#include <open_xr/openxr.h>

// Other includes
#include <vector>

struct swapchain_data_t {
  ID3D11DepthStencilView *depth_buffer;
  ID3D11RenderTargetView *back_buffer;
};

struct swapchain_t {
  XrSwapchain handle;
  int32_t width;
  int32_t height;
  std::vector<swapchain_data_t> swapchain_data;
};

struct vertex {
  float x, y, z;
  DirectX::XMFLOAT4 color;
};
