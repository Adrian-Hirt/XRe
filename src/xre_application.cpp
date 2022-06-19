#include <xre/xre_application.h>

XReApplication::XReApplication(const char *application_name) {
  // create the handler
  xre_open_xr_handler = XrOpenXrHandler(application_name);

  bool result;

  // Instruct the handler to initialize the xr system
  result = xre_open_xr_handler.initialize_xr_system();

  // Check if we were successful in creating the openxr system
  if(!result) {
    std::cout << "Initializing the OpenXR system failed" << std::endl;
  }
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