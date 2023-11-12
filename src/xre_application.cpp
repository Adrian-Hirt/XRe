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

  // Call the setup mmethod
  setup();

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

void XReApplication::setup() {
  // Override this method to setup your scene etc.
}

void XReApplication::draw(XrCompositionLayerProjectionView &view) {
  // Override this method to draw some stuff
}

ID3D11Device* XReApplication::get_device() {
  return xre_open_xr_handler.get_device();
};

ID3D11DeviceContext* XReApplication::get_device_context() {
  return xre_open_xr_handler.get_device_context();
};
