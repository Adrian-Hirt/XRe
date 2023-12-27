#include <xre/application.h>

Application::Application(const char *application_name) {
  // create the XR handler
  open_xr_handler = OpenXrHandler(application_name);
  dx11_handler = open_xr_handler.dx11_handler;
};

Application::~Application() {};

void Application::run() {
  bool loop_running = true;
  bool xr_running = false;

  // Call the setup mmethod
  setup();

  // Main render loop
  while(loop_running) {
    // Poll the OpenXR events, and if OpenXR reports to still be running, keep going on
    open_xr_handler.pollOpenxrEvents(loop_running, xr_running);

    if (xr_running) {
      // Render frame
      open_xr_handler.renderFrame(std::bind(&Application::draw, this), std::bind(&Application::updateSimulation, this, std::placeholders::_1));
    }
  }
};

void Application::setup() {
  // Override this method to setup your scene etc.
};

void Application::draw() {
  // Override this method to draw some stuff
};

void Application::updateSimulation(XrTime predicted_time) {
  // Override this method to update the simulation based
  // on the predicted time the frame will be rendered
}

ID3D11Device* Application::getDevice() {
  return open_xr_handler.getDevice();
};

ID3D11DeviceContext* Application::getDeviceContext() {
  return open_xr_handler.getDeviceContext();
};

void Application::setCcwCullMode() {
  dx11_handler.useDefaultRasterizer(false);
};

void Application::setCwCullMode() {
  dx11_handler.useDefaultRasterizer(true);
};

void Application::setWireframeMode() {
  dx11_handler.useWireframeRasterizer();
};
