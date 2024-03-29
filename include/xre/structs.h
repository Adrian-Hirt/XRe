#pragma once

// DirectX includes
#include <d3d11.h>
#include <DirectXMath/DirectXMath.h>

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

typedef struct vertex_t {
  DirectX::XMFLOAT3 coordinates;
  DirectX::XMFLOAT3 normal;
  DirectX::XMFLOAT2 texture_coordinates;
} vertex_t;

typedef struct per_frame_const_buffer_t {
  DirectX::XMMATRIX view_projection;
} per_frame_const_buffer_t;

typedef struct lighting_const_buffer_t {
  DirectX::XMFLOAT4 light_vector;
  DirectX::XMFLOAT4 light_color;
  DirectX::XMFLOAT4 ambient_color;
} lighting_const_buffer_t;

typedef struct per_model_const_buffer_t {
  DirectX::XMMATRIX model;
  DirectX::XMMATRIX normal_rotation;
  DirectX::XMFLOAT4 color;
} per_model_const_buffer_t;

typedef struct text_char_t {
  float left;
  float right;
  float top;
  float bottom;
} text_char_t;
