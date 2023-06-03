#include <xre/xre_application.h>

XReApplication::XReApplication(const char *application_name) {
  // create the handler
  xre_open_xr_handler = XrOpenXrHandler(application_name);
};

XReApplication::~XReApplication() {

};

void XReApplication::run() {
  bool loop_running = true;
  bool xr_running = false;

  // Main render loop
  while(loop_running) {
    // Poll the OpenXR events, and if OpenXR reports to still be running, keep going on
    xre_open_xr_handler.poll_openxr_events(loop_running, xr_running);

    if (xr_running) {
      // Poll openXR actions
      xre_open_xr_handler.poll_openxr_actions();

      // Render frame
      xre_open_xr_handler.render_frame(std::bind(&XReApplication::draw, this, std::placeholders::_1));
    }
  }
}

void XReApplication::draw(XrCompositionLayerProjectionView &view) {
  // Override this method to draw some stuff
}
