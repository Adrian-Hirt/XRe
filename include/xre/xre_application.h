#pragma once

// XRe includes
#include <xre/utils.h>
#include <xre/xre_openxr_handler.h>

// Other includes
#include <iostream>

class XReApplication {
public:
  XReApplication(const char *application_name);
  ~XReApplication();

  void run();

private:
  XrOpenXrHandler xre_open_xr_handler;
};
