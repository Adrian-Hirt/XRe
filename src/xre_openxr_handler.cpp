#include <xre/xre_openxr_handler.h>

// Empty default constructor
XrOpenXrHandler::XrOpenXrHandler() {};

XrOpenXrHandler::XrOpenXrHandler(const char *application_name) {
  this->application_name = application_name;
};

XrOpenXrHandler::~XrOpenXrHandler() {

};

bool XrOpenXrHandler::initialize_xr_system() {
  XrResult result;

  // First, setup the OpenXR instance. We're only using the DirectX11 extension for now.
  // other extensions might be added later.
  const char* extensions[] = { XR_KHR_D3D11_ENABLE_EXTENSION_NAME };

  // Create the param struct
  XrInstanceCreateInfo create_info = {};
	create_info.type = XR_TYPE_INSTANCE_CREATE_INFO;
	create_info.enabledExtensionCount = 1;
	create_info.enabledExtensionNames = extensions;
	create_info.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
	strcpy_s(create_info.applicationInfo.applicationName, 128, this->application_name);

	result = xrCreateInstance(&create_info, &openxr_instance);
	if (XR_FAILED(result)) {
		return false;
	}

  return true;
}