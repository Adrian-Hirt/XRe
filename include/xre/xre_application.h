#include <iostream>
#include <xre/xre_openxr_handler.h>

class XReApplication {
public:
  XReApplication(const char *application_name);
  ~XReApplication();

  void run();

private:
  XrOpenXrHandler xre_open_xr_handler;
};
