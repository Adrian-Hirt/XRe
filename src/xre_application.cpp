#include <xre/xre_application.h>

XReApplication::XReApplication(const char *application_name) {
  // create the handler
  xre_open_xr_handler = XrOpenXrHandler(application_name);
};

XReApplication::~XReApplication() {

};

void XReApplication::run() {

  // Main render loop
  while(true) {

    // Just exit directly for now
    break;
  }
}