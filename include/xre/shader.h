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
  Shader(const char *shader_path, ID3D11Device *device, ID3D11DeviceContext *device_context);

  void activate();
  void cleanUp();
  void updateConstantBuffer();

  static void setViewProjectionMatrix(DirectX::XMMATRIX view_projection);
  void setModelMatrix(DirectX::XMMATRIX model_matrix);
  void setNormalRotationMatrix(DirectX::XMMATRIX rotation_matrix);

  inline static Shader* getCurrentActiveShader() { return current_active_shader; };

private:
  // Shader objects
  ID3D11VertexShader *vertex_shader;
  ID3D11PixelShader *pixel_shader;

  // Input layout
  ID3D11InputLayout *input_layout;

  // D3D11 device and device context
  ID3D11Device *device;
  ID3D11DeviceContext *device_context;

  // Keep track of the currently activated shader
  inline static Shader *current_active_shader = NULL;

  // Keep track of the current ViewProjection matrix, which
  // is shared between all shaders
  inline static DirectX::XMMATRIX view_projection = DirectX::XMMatrixIdentity();

  // Constant buffer pointer & the constant buffer itself
  ID3D11Buffer *p_const_buffer;
  const_buffer_t const_buffer;
};