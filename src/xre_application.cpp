#include <xre/xre_application.h>

XReApplication::XReApplication(const char *application_name) {
  // create the handler
  xre_open_xr_handler = XrOpenXrHandler(application_name);
};

XReApplication::~XReApplication() {

};

void XReApplication::run() {
  std::cout << "Hello from the run() method" << std::endl;

  bool loop_running = true;
  bool xr_running = false;

  // Main render loop
  while(loop_running) {
    // Poll the OpenXR events, and if OpenXR reports to still be running, keep going on
    xre_open_xr_handler.poll_openxr_events(loop_running, xr_running);

    if (xr_running) {
      // TODO:
      // 1. Poll openXR actions
      // 2. Render frame
    }
  }
}