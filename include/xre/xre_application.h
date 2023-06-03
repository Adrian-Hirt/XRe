#pragma once

// XRe includes
#include <xre/utils.h>
#include <xre/xre_openxr_handler.h>

// Other includes
#include <iostream>
#include <functional>

class XReApplication {
public:
  XReApplication(const char *application_name);
  ~XReApplication();

  void run();
  virtual void draw(XrCompositionLayerProjectionView &view);

private:
  XrOpenXrHandler xre_open_xr_handler;
};
