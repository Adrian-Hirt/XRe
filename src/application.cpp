#include <xre/application.h>

Application::Application(const char *application_name) {
  // create the XR handler
  m_open_xr_handler = std::make_unique<OpenXrHandler>(application_name);


  // Create the resource manager
  m_resource_manager = std::make_shared<ResourceManager>(m_open_xr_handler->m_vulkan_handler);
}

Application::~Application() {};

void Application::run() {
  bool loop_running = true;
  bool xr_running = false;

  // Call the setup method
  setup();

  // Main render loop
  while (loop_running) {
    // Poll the OpenXR events, and if OpenXR reports to still be running, keep going on
    m_open_xr_handler->pollOpenxrEvents(loop_running, xr_running);

    if (xr_running) {
      // Render frame
      m_open_xr_handler->renderFrame(std::bind(&Application::draw, this, std::placeholders::_1),
                                    std::bind(&Application::updateSimulation, this, std::placeholders::_1));
    }
  }
}

void Application::setup() {
  // Override this method to setup your scene etc.
}

void Application::draw(RenderContext &ctx) {
  // Override this method to draw some stuff
}

void Application::updateSimulation(XrTime predicted_time) {
  // Override this method to update the simulation based
  // on the predicted time the frame will be rendered
}

ResourceManager& Application::resourceManager() {
  return *m_resource_manager;
};
