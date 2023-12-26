#pragma once

// XRe includes
#include <xre/utils.h>
#include <xre/geometry.h>
#include <xre/shader.h>
#include <xre/line.h>
#include <xre/mesh.h>
#include <xre/model.h>
#include <xre/openxr_handler.h>
#include <xre/model_factory.h>

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

  void setCcwCullMode();
  void setCwCullMode();
  void setWireframeMode();

  ModelFactory model_factory;

private:
  // Handlers
  OpenXrHandler open_xr_handler;
  Dx11Handler dx11_handler;
};
