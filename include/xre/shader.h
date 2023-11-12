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

  void render(ID3D11DeviceContext *device_context);

private:
  ID3D11VertexShader *vertex_shader;
  ID3D11PixelShader *pixel_shader;
  ID3D11InputLayout *input_layout;
  ID3D11Buffer *vertex_buffer;
};