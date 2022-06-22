#include <xre/xre_openxr_handler.h>

//------------------------------------------------------------------------------------------------------
// Empty default constructor
//------------------------------------------------------------------------------------------------------
XrOpenXrHandler::XrOpenXrHandler() {};

//------------------------------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------------------------------
XrOpenXrHandler::XrOpenXrHandler(const char *application_name) {
  this->application_name = application_name;

  bool result;

  // Instruct the handler to initialize the xr system
  result = initialize_openxr();

  // Check if we were successful in creating the openxr system.
  check_bool_result(result, "Initializing OpenXR failed!");

  // Instruct the handler to initialize the xr actions
  result = initialize_openxr_actions();

  // Check if we were successful in creating the openxr actions.
  check_bool_result(result, "Initializing the OpenXR actions failed!");
};

//------------------------------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------------------------------
XrOpenXrHandler::~XrOpenXrHandler() {};

//------------------------------------------------------------------------------------------------------
// Initialize the general OpenXR stuff
//------------------------------------------------------------------------------------------------------
bool XrOpenXrHandler::initialize_openxr() {
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

	// Create a new handler for the DirectX 11 related stuff. Somehow this does not work
  // if we don't store it as a pointer to the actual handler.
  XreDx11Handler handler = XreDx11Handler(graphics_requirements.adapterLuid);
	this->xr_dx11_handler = &handler;

	// Create a binding for the D3D11 device
	XrGraphicsBindingD3D11KHR graphics_binding = {};
	graphics_binding.type = XR_TYPE_GRAPHICS_BINDING_D3D11_KHR;
	graphics_binding.device = xr_dx11_handler->get_device();

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
	// For each viewport, we need to setup a swapchain. A swapchain consists of multiple buffers, where one
	// is used to draw the data to the screen, and another is used to render the deta from the simulation
	// to. With this approach, tearing (that might occur because the scene is updated while it's drawn
	// to the screen) should not occur
	for (uint32_t i = 0; i < viewport_count; i++) {
		// Get the current view configuration we're interested in
		XrViewConfigurationView& current_view_configuration = openxr_view_configuration_views[i];

		// Create a create info struct to create the swapchain
		XrSwapchainCreateInfo swapchain_create_info = {};
		swapchain_create_info.type = { XR_TYPE_SWAPCHAIN_CREATE_INFO };
		swapchain_create_info.arraySize = 1; // Number of array layers
		swapchain_create_info.mipCount = 1; // Only use one mipmap level, bigger numbers would only be useful for textures
		swapchain_create_info.faceCount = 1; // Number of faces to render, 1 should be used, other option would be 6 for cubemaps
		swapchain_create_info.format = xr_dx11_handler->d3d11_swapchain_format;
		swapchain_create_info.width = current_view_configuration.recommendedImageRectWidth; // Just use the recommended width that the runtime gave us
		swapchain_create_info.height = current_view_configuration.recommendedImageRectHeight; // Just use the recommended height that the runtime gave us
		swapchain_create_info.sampleCount = current_view_configuration.recommendedSwapchainSampleCount; // Just use the recommended sample count that the runtime gave us
		swapchain_create_info.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;

		// Create the OpenXR swapchain
		XrSwapchain swapchain_handle;
		result = xrCreateSwapchain(openxr_session, &swapchain_create_info, &swapchain_handle);
		if (XR_FAILED(result)) {
			return false;
		}

		// OpenXR can create an arbitrary number of swapchain images (from which we'll create our buffers),
		// so we need to find out how many were created by the runtime
		// If we pass zero as the 2nd param, we request the number of swapchain images and store it
		// in the 3rd param
		uint32_t swapchain_image_count = 0;
		result = xrEnumerateSwapchainImages(swapchain_handle, 0, &swapchain_image_count, NULL);
		if (XR_FAILED(result)) {
			return false;
		}

		// We need a vector to store the swapchain images. The swapchain images store
		// the image data in a structured way. As we only need these to create the swapchains,
		// we put them in a temporary vector which we won't use afterwards
		std::vector<XrSwapchainImageD3D11KHR> swapchain_images;
		swapchain_images.resize(swapchain_image_count, {XR_TYPE_SWAPCHAIN_IMAGE_D3D11_KHR});


		// Now we can create the swapchain struct, that stores the swapchain handle, the height
		// and width of the swapchains (which usually should match the size of the window / view
		// we render to, and the swapchain_data structs (which contain the backbuffer & depthbuffer
		// of the many swapchains we get)
		swapchain_t swapchain = {};

		swapchain.width = swapchain_create_info.width;
		swapchain.height = swapchain_create_info.height;
		swapchain.handle = swapchain_handle;
		swapchain.swapchain_data.resize(swapchain_image_count);

		// Now call the xrEnumerateSwapchainImages function again, this time with the 2nd param set to the number
		// of swapchain images that got created by OpenXR. That way, we can store the swapchain images into our
		// temporary vector and use them to create the swapchains
		result = xrEnumerateSwapchainImages(swapchain_handle, swapchain_image_count, &swapchain_image_count, (XrSwapchainImageBaseHeader *)swapchain_images.data());
		if (XR_FAILED(result)) {
      return false;
		}

    // For each swapchain image, call the function to create a render target using that swapchain image
    // We also directly release the texture object, as we don't need it anymore after we created the
    // render target with it
    for (uint32_t i = 0; i < swapchain_image_count; i++) {
        swapchain.swapchain_data[i] = xr_dx11_handler->create_render_targets(*swapchain_images[i].texture);
        swapchain_images[i].texture->Release();
    }

    // We're done creating that swapchain, we can now add it to the vector of our swapchains (as we have multiple,
    // as mentioned before one for each view
    swapchains.push_back(swapchain);
	}

  return true;
}

//------------------------------------------------------------------------------------------------------
// Initialize the OpenXR actions
//------------------------------------------------------------------------------------------------------
bool XrOpenXrHandler::initialize_openxr_actions() {
  // TODO: implement some basic actions
  return true;
}

//------------------------------------------------------------------------------------------------------
// Poll the OpenXR events
//------------------------------------------------------------------------------------------------------
void XrOpenXrHandler::poll_openxr_events(bool &loop_running, bool &xr_running) {
  XrResult result;

  // Struct to save the event data in.
	// We'll pass this to the xrPollEvent call, where the details of the
	// event will be stored in thhis struct.
	XrEventDataBuffer event_data_buffer = { XR_TYPE_EVENT_DATA_BUFFER };

	// Keep polling events as long as xrPollEvent returns events.
	// The call to xrPollEvent fetches the "next event" of the xr_instance,
	// and stores the event data inside the event data buffer we created earlier.
	// We use the XR_UNQUALIFIED_SUCCESS macro to see if the xrPollEvent call
	// was successful (i.e. was able to fetch an event)
	// If there are no more events left over, we can end.
	while (XR_UNQUALIFIED_SUCCESS(xrPollEvent(openxr_instance, &event_data_buffer))) {
    // Instance seems to shut down, so we need to exit right away
    if (event_data_buffer.type == XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING) {
      loop_running = false;
      return;
		}

    // The state of the session was changed, we might need to do something here
    if (event_data_buffer.type == XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED) {
			XrEventDataSessionStateChanged* state_change = (XrEventDataSessionStateChanged*)&event_data_buffer;

      switch(state_change->state) {
        // See https://www.khronos.org/files/openxr-10-reference-guide.pdf for the relevant states
        case XR_SESSION_STATE_READY: {
          // Session is in the READY state, i.e. we can begin the session to enter the SYNCHRONIZED state
          XrSessionBeginInfo session_begin_info = {};
					session_begin_info.type = XR_TYPE_SESSION_BEGIN_INFO;
					session_begin_info.primaryViewConfigurationType = application_view_type;

					// Start the session. If the call was successful, set the xr_running flag
					// to true
					result = xrBeginSession(openxr_session, &session_begin_info);
          check_xr_result(result, "Couldn't begin the XR session.");
          xr_running = true;
					break;
        }
        case XR_SESSION_STATE_STOPPING: {
          // In STOPPING state, i.e. we need to end the session
          result = xrEndSession(openxr_session);
          check_xr_result(result, "Couldn't end the XR session.");
          xr_running = false;
          break;
        }
        case XR_SESSION_STATE_EXITING: {
          // App exited quit the main loop of the app
          loop_running = false;
          break;
        }
        case XR_SESSION_STATE_LOSS_PENDING: {
          // Similar to previously, instance seems to be shutting down (or losing connection
          // or whatever), so we need to quit the main loop
          loop_running = false;
          break;
        }
      }
    }

    // "Reset" the event data buffer, such that it's ready for the next event
		// being fetched (if there is any to be fetched)
		event_data_buffer = { XR_TYPE_EVENT_DATA_BUFFER };
  }
}

