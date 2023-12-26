#pragma once

// DirectX includes
#include <d3d11.h>
#include <d3dcompiler.h>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>

class Shader {
public:
  Shader();
  Shader(const char *shader_path);

  void activate();
  void cleanUp();
  void updatePerModelConstantBuffer();

  void setModelMatrix(DirectX::XMMATRIX model_matrix);
  void setNormalRotationMatrix(DirectX::XMMATRIX rotation_matrix);

  inline static Shader* getCurrentActiveShader() { return current_active_shader; };
  static void createGlobalBuffers(ID3D11Device *device, ID3D11DeviceContext *device_context);
  static void updateViewProjectionMatrix(DirectX::XMMATRIX view_projection);
  static void registerDx11DeviceAndDeviceContext(ID3D11Device *device, ID3D11DeviceContext *device_context);

private:
  // Shader objects
  ID3D11VertexShader *vertex_shader;
  ID3D11PixelShader *pixel_shader;

  // Input layout
  ID3D11InputLayout *input_layout;

  // D3D11 device and device context
  inline static ID3D11Device *device = NULL;
  inline static ID3D11DeviceContext *device_context = NULL;

  // Keep track of the currently activated shader
  inline static Shader *current_active_shader = NULL;

  // Global buffer to keep the viewprojection matrix, which should
  // only be updated once per frame, regardless of the number of
  // shaders
  inline static per_frame_const_buffer_t global_per_frame_const_buffer = {};
  inline static ID3D11Buffer *p_global_per_frame_const_buffer = NULL;

  // Global buffer to keep the lighting, which only should be updated
  // when the lighting changes.
  inline static lighting_const_buffer_t lighting_const_buffer = {};
  inline static ID3D11Buffer *p_lighting_const_buffer = NULL;

  // Constant buffer pointers & the constant buffers themselfes.
  ID3D11Buffer *p_per_model_const_buffer;
  per_model_const_buffer_t per_model_const_buffer;
};