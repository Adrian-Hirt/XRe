#pragma once

// XRe includes
#include <xre/utils.h>
#include <xre/geometry.h>
#include <xre/shader.h>
#include <xre/mesh.h>
#include <xre/model.h>
#include <xre/openxr_handler.h>

// Other includes
#include <iostream>
#include <functional>

class Application {
public:
  Application(const char *application_name);
  ~Application();

  void run();
  virtual void setup();
  virtual void draw(XrCompositionLayerProjectionView &view);

  ID3D11Device* getDevice();
  ID3D11DeviceContext* getDeviceContext();

private:
  OpenXrHandler open_xr_handler;
};
