#include <xre/application.h>

Application::Application(const char *application_name) {
  // create the XR handler
  m_open_xr_handler = OpenXrHandler(application_name);
  // m_dx11_handler = &m_open_xr_handler.m_dx11_handler;

  // // Register the handler with the model class
  // Model::registerDx11Handler(m_dx11_handler);
}

Application::~Application() {};

void Application::run() {
  bool loop_running = true;
  bool xr_running = false;

  // Call the setup method
  setup();

  // Main render loop
  while(loop_running) {
    // Poll the OpenXR events, and if OpenXR reports to still be running, keep going on
    m_open_xr_handler.pollOpenxrEvents(loop_running, xr_running);

    if (xr_running) {
      // Render frame
      m_open_xr_handler.renderFrame(
        std::bind(&Application::draw, this, std::placeholders::_1), 
        std::bind(&Application::updateSimulation, this, std::placeholders::_1)
      );
    }
  }
}

void Application::setup() {
  // Override this method to setup your scene etc.
}

void Application::draw(VkCommandBuffer command_buffer) {
  // Override this method to draw some stuff
}

void Application::updateSimulation(XrTime predicted_time) {
  // Override this method to update the simulation based
  // on the predicted time the frame will be rendered
}

// ID3D11Device* Application::getDevice() {
//   return m_open_xr_handler.getDevice();
// }

// ID3D11DeviceContext* Application::getDeviceContext() {
//   return m_open_xr_handler.getDeviceContext();
// }

// void Application::setCcwCullMode() {
//   m_dx11_handler->useDefaultRasterizer(false);
// }

// void Application::setCwCullMode() {
//   m_dx11_handler->useDefaultRasterizer(true);
// }

// void Application::setWireframeMode() {
//   m_dx11_handler->useWireframeRasterizer();
// }
