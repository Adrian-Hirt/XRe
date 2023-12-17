#pragma once

// XRe includes
#include <xre/utils.h>
#include <xre/geometry.h>
#include <xre/shader.h>
#include <xre/mesh.h>
#include <xre/xre_openxr_handler.h>

// Other includes
#include <iostream>
#include <functional>

class XReApplication {
public:
  XReApplication(const char *application_name);
  ~XReApplication();

  void run();
  virtual void setup();
  virtual void draw(XrCompositionLayerProjectionView &view);

  ID3D11Device* get_device();
  ID3D11DeviceContext* get_device_context();

private:
  XrOpenXrHandler xre_open_xr_handler;
};
