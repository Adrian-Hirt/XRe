#pragma once

// DirectX includes
#include <d3d11.h>

// XRe includes
#include <xre/utils.h>

class XreDx11Handler {
public:
  XreDx11Handler();
  XreDx11Handler(LUID &adapter_luid);
  ~XreDx11Handler();

  ID3D11Device* get_device();
  ID3D11DeviceContext* get_device_context();

private:
  ID3D11Device *device;
  ID3D11DeviceContext *device_context;

  // Methods
  bool initialize_device(LUID &adapter_luid);
};
