#include <xre/application.h>

Application::Application(const char *application_name) {
  // create the XR handler
  open_xr_handler = OpenXrHandler(application_name);

  // Create the model factory
  model_factory = ModelFactory();

  // Initialize the global buffers for the shaders
  Shader::createGlobalBuffers(getDevice(), getDeviceContext());

  // Register the device & device_context with the classes that need them
  Shader::registerDx11DeviceAndDeviceContext(getDevice(), getDeviceContext());
  Mesh::registerDx11DeviceAndDeviceContext(getDevice(), getDeviceContext());
  Model::registerDx11DeviceAndDeviceContext(getDevice(), getDeviceContext());
};

Application::~Application() {

};

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
      // Poll openXR actions
      open_xr_handler.pollOpenxrActions();

      // Render frame
      open_xr_handler.renderFrame(std::bind(&Application::draw, this, std::placeholders::_1));
    }
  }
}

void Application::setup() {
  // Override this method to setup your scene etc.
}

void Application::draw(XrCompositionLayerProjectionView &view) {
  // Override this method to draw some stuff
}

ID3D11Device* Application::getDevice() {
  return open_xr_handler.getDevice();
};

ID3D11DeviceContext* Application::getDeviceContext() {
  return open_xr_handler.getDeviceContext();
};
