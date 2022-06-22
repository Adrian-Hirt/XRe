#include <xre/xre_openxr_handler.h>

// Empty default constructor
XrOpenXrHandler::XrOpenXrHandler() {};

XrOpenXrHandler::XrOpenXrHandler(const char *application_name) {
  this->application_name = application_name;

  bool result;

  // Instruct the handler to initialize the xr system
  result = initialize_open_xr();

  // Check if we were successful in creating the openxr system.
  check_bool_result(result, "Initializing OpenXR failed!");
};

XrOpenXrHandler::~XrOpenXrHandler() {

};

bool XrOpenXrHandler::initialize_open_xr() {
  XrResult result;

	//------------------------------------------------------------------------------------------------------
	// OpenXR Instance
	//------------------------------------------------------------------------------------------------------
  // First, setup the OpenXR instance. We're only using the DirectX11 extension for now.
  // other extensions might be added later.
  const char* extensions[] = { XR_KHR_D3D11_ENABLE_EXTENSION_NAME };

  // Create the param struct
  XrInstanceCreateInfo instance_create_info = {};
	instance_create_info.type = XR_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.enabledExtensionCount = 1;
	instance_create_info.enabledExtensionNames = extensions;
	instance_create_info.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
	strcpy_s(instance_create_info.applicationInfo.applicationName, 128, this->application_name);

	result = xrCreateInstance(&instance_create_info, &openxr_instance);
	if (XR_FAILED(result)) {
		return false;
	}

	//------------------------------------------------------------------------------------------------------
	// OpenXR System
	//------------------------------------------------------------------------------------------------------
	// Next, setup the OpenXR system, which is rather simple
	XrSystemGetInfo system_get_info = {};
	system_get_info.type = XR_TYPE_SYSTEM_GET_INFO;
	system_get_info.formFactor = application_form_factor;
	result = xrGetSystem(openxr_instance, &system_get_info, &openxr_system_id);
	if (XR_FAILED(result)) {
		return false;
	}

	//------------------------------------------------------------------------------------------------------
	// OpenXR Session
	//------------------------------------------------------------------------------------------------------
	// Get the blend modes for the XR device. As the function call to xrEnumerateEnvironmentBlendModes
	// should return the blend modes in order of preference of the runtime, we can just pick the first one
	uint32_t blend_count = 0; // Throwaway variable, but we need to pass in a pointer to a uint32_t, or the function call fails
	result = xrEnumerateEnvironmentBlendModes(openxr_instance, openxr_system_id, application_view_type, 1, &blend_count, &openxr_blend_mode);
	if (XR_FAILED(result)) {
		return false;
	}

	// Get the address of the "xrGetD3D11GraphicsRequirementsKHR" function and store it, such that we can
	// call the function. As of now, it seems it's not yet possible to directly call the function
	xrGetInstanceProcAddr(openxr_instance, "xrGetD3D11GraphicsRequirementsKHR", (PFN_xrVoidFunction*)(&ext_xrGetD3D11GraphicsRequirementsKHR));

	XrGraphicsRequirementsD3D11KHR graphics_requirements = {};
	graphics_requirements.type = XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR;
	// Call the function which retrieves the D3D11 feature level and graphic device requirements for an instance and system
	// The graphics_requirements param is a XrGraphicsRequirementsD3D11KHR struct, where
	// the field "adapterLuid" identifies the graphics device to be used, and the field "minFeatureLevel" specifies
	// the minimum feature level the D3D11 device must be initialized with
	result = ext_xrGetD3D11GraphicsRequirementsKHR(openxr_instance, openxr_system_id, &graphics_requirements);
	if (XR_FAILED(result)) {
		return false;
	}

	// Create a new handler for the DirectX 11 related stuff
	XreDx11Handler xr_dx11_handler = XreDx11Handler(graphics_requirements.adapterLuid);

	// Create a binding for the D3D11 device
	XrGraphicsBindingD3D11KHR graphics_binding = {};
	graphics_binding.type = XR_TYPE_GRAPHICS_BINDING_D3D11_KHR;
	graphics_binding.device = xr_dx11_handler.get_device();

	// Create the session info struct
	XrSessionCreateInfo session_create_info = {};
	session_create_info.type = XR_TYPE_SESSION_CREATE_INFO;
	session_create_info.next = &graphics_binding;
	session_create_info.systemId = openxr_system_id;

	// And finally create the openxr session
	result = xrCreateSession(openxr_instance, &session_create_info, &openxr_session);
	if (XR_FAILED(result)) {
		return false;
	}

	//------------------------------------------------------------------------------------------------------
	// Reference Spaces
	//------------------------------------------------------------------------------------------------------
	// Next, we need to choose a reference space to display the content. We'll use stage, which is relative
	// to the devices "guardian" system
	XrReferenceSpaceCreateInfo reference_space_create_info = {};
	reference_space_create_info.type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO;
	reference_space_create_info.poseInReferenceSpace = { {0, 0, 0, 1}, {0, 0, 0} };
	reference_space_create_info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;

	// Create the reference space
	result = xrCreateReferenceSpace(openxr_session, &reference_space_create_info, &openxr_space);
	if (XR_FAILED(result)) {
		return false;
	}

	//------------------------------------------------------------------------------------------------------
	// View ports
	//------------------------------------------------------------------------------------------------------
	// Devices running OpenXR code can have multiple viewports (views we need to render). For a stereo
	// headset (XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO), this is usually 2, one for each eye. If it's
	// an AR application running on e.g. a phone, this would be 1, and for other constellations, such as
	// a cave-like system where on each wall an image is projected, this can even be 6 (or any other number).

	// As such, we first need to find out how many views we need to support. If we set the 4th param of
	// the xrEnumerateViewConfigurationViews function call to zero, it will retrieve the required
	// number of viewports and store it in the 5th param
	uint32_t viewport_count = 0;
	result = xrEnumerateViewConfigurationViews(openxr_instance, openxr_system_id, application_view_type, 0, &viewport_count, NULL);
	if (XR_FAILED(result)) {
		return false;
	}

	// Now that we know how many views we need to render, resize the vector that contains the view
	// configurations (each XrViewConfigurationView specifies properties related to rendering an
	// individual view) and the vector containing the views (each XrView specifies the pose and
	// the fov of the view. Basically, a XrView is a view matrix in "traditional" rendering).
	openxr_view_configuration_views.resize(viewport_count, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
	openxr_views.resize(viewport_count, { XR_TYPE_VIEW });

	// Now we again call xrEnumerateViewConfigurationViews, this time we set the 4th param
	// to the number of our viewports, such that the method fills the xr_view_configurations
	// vector with the actual view configurations
	result = xrEnumerateViewConfigurationViews(openxr_instance, openxr_system_id, application_view_type, viewport_count, &viewport_count, openxr_view_configuration_views.data());
	if (XR_FAILED(result)) {
		return false;
	}

	//------------------------------------------------------------------------------------------------------
	// Swapchains
	//------------------------------------------------------------------------------------------------------

	std::cout << "Done" << std::endl;

  return true;
}