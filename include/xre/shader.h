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

  void render();
  void activate();
  void setVertexData(vertex vertex_data[], size_t vertex_data_size);
  void cleanUp();

private:
  // Shader objects
  ID3D11VertexShader *vertex_shader;
  ID3D11PixelShader *pixel_shader;

  // Input layout
  ID3D11InputLayout *input_layout;

  // Vertex buffer
  ID3D11Buffer *vertex_buffer;

  // D3D11 device and device context
  ID3D11Device *device;
  ID3D11DeviceContext *device_context;

  // Keep track of the currently activated shader
  inline static Shader *current_active_shader = NULL;
};