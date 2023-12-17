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

	// Create a new handler for the DirectX 11 related stuff.
 	dx11_handler = Dx11Handler(graphics_requirements.adapterLuid);

	// Create a binding for the D3D11 device
	XrGraphicsBindingD3D11KHR graphics_binding = {};
	graphics_binding.type = XR_TYPE_GRAPHICS_BINDING_D3D11_KHR;
	graphics_binding.device = dx11_handler.getDevice();

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
		swapchain_create_info.format = dx11_handler.d3d11_swapchain_format;
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
        swapchain.swapchain_data[i] = dx11_handler.createRenderTargets(*swapchain_images[i].texture);
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

//------------------------------------------------------------------------------------------------------
// Poll the OpenXR actions
//------------------------------------------------------------------------------------------------------
void XrOpenXrHandler::poll_openxr_actions() {
	// This is not doing anything for the moment
}

//------------------------------------------------------------------------------------------------------
// Renders the next frame
//------------------------------------------------------------------------------------------------------
void XrOpenXrHandler::render_frame(std::function<void(XrCompositionLayerProjectionView&)> draw_callback) {
	XrResult result;

	//------------------------------------------------------------------------------------------------------
	// Setup the frame
	//------------------------------------------------------------------------------------------------------
	// The call to xrWait frame will fill in the frame_state struct, where the field we'll
	// be interested in is the predictedDisplayTime field.
	// That field stores a prediction when the next frame will be displayed. This can be used to
	// place objects, viewpoints, controllers etc. in the view
	XrFrameState xr_frame_state = {};
	xr_frame_state.type = XR_TYPE_FRAME_STATE;
	result = xrWaitFrame(openxr_session, NULL, &xr_frame_state);
	if(XR_FAILED(result)) {
		return;
	}

	//------------------------------------------------------------------------------------------------------
	// Begin the frame
	//------------------------------------------------------------------------------------------------------
	result = xrBeginFrame(openxr_session, NULL);
	if (XR_FAILED(result)) {
		return;
	}

	//------------------------------------------------------------------------------------------------------
	// Update simulation
	//------------------------------------------------------------------------------------------------------
	// TODO: here we'll need to update the simulation, not sure how yet though

	//------------------------------------------------------------------------------------------------------
	// Render the layer
	//------------------------------------------------------------------------------------------------------
	std::vector<XrCompositionLayerBaseHeader*> layers;
	XrCompositionLayerProjection layer_projection = {};
	layer_projection.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION;
	std::vector<XrCompositionLayerProjectionView> views;


	// Check if the xrSession is in a state that we actually need to render. If the session isn't in the
	// VISIBLE or in the FOCUSED state, we don't need to render the layer (e.g. when the user of the
	// application takes off the vr headset while the application still is running. In that case,
	// we need to keep the application (and the simulation) running, but there is no point in rendering
	// anything.
	if (xr_frame_state.shouldRender) {
		render_layer(xr_frame_state.predictedDisplayTime, views, layer_projection, draw_callback);
		layers.push_back((XrCompositionLayerBaseHeader *)&layer_projection);
	}

	//------------------------------------------------------------------------------------------------------
	// Done rendeding the layer, send it to the display
	//------------------------------------------------------------------------------------------------------
	XrFrameEndInfo frame_end_info = {};
	frame_end_info.type = XR_TYPE_FRAME_END_INFO;
	frame_end_info.displayTime = xr_frame_state.predictedDisplayTime;
	frame_end_info.environmentBlendMode = openxr_blend_mode;
	frame_end_info.layerCount = layers.size();
	frame_end_info.layers = layers.data();
	xrEndFrame(openxr_session, &frame_end_info);
}

//------------------------------------------------------------------------------------------------------
// Renders an OpenXR layer
//------------------------------------------------------------------------------------------------------
void XrOpenXrHandler::render_layer(XrTime predicted_time, std::vector<XrCompositionLayerProjectionView>& views,
																	 XrCompositionLayerProjection& layer_projection,
																	 std::function<void(XrCompositionLayerProjectionView&)> draw_callback) {
	XrResult result;

	uint32_t view_count = 0;

	//------------------------------------------------------------------------------------------------------
	// Setup the views for the predicted rendering time
	//------------------------------------------------------------------------------------------------------
	// We got the predicted time from the OpenXR runtime at which it will render the next frame (i.e. the
	// frame we're preparing to render right now.
	// We can use this predicted time to call the method xrLocateViews, which will return the view and
	// projection info for a particular time.
	XrViewState view_state = {};
	view_state.type = XR_TYPE_VIEW_STATE;

	// Setup an info struct which we'll pass into the xrLocateView call with informations about the
	// predicted time, the type of view we have and the xr space we're in
	XrViewLocateInfo view_locate_info = {};
	view_locate_info.type = XR_TYPE_VIEW_LOCATE_INFO;
	view_locate_info.viewConfigurationType = application_view_type;
	view_locate_info.displayTime = predicted_time;
	view_locate_info.space = openxr_space;

	// Call xrLocateViews, which will give us the number of views we have to render (stored in view_count), as
	// well as fill in the xr_views vector with the predicted views (which is basically a struct containing
	// the pose of the view, as well as the fov for that view. We'll use these two later to render with D3D11,
	// as we need to modify the objects and the view before rendering.
	result = xrLocateViews(openxr_session, &view_locate_info, &view_state, (uint32_t)openxr_views.size(), &view_count, openxr_views.data());
	check_xr_result(result, "Could not locate views!");
	views.resize(view_count);

	//------------------------------------------------------------------------------------------------------
	// Render the layer for each view
	//------------------------------------------------------------------------------------------------------
	for (uint32_t i = 0; i < view_count; i++) {
		// First, we need to acquire a swapchain image, as we need a render target to render the data
		// to. As a reminder (from the CreateSwapchainRenderTargets method), a swapchain image
		// in the context of D3D11 is the buffer we want to render to.
		// As we don't pass a swapchain_image_id into the xrAcquireSwapchainImage call, the runtime decides
		// which swapchain image we'll get
		uint32_t swapchain_image_id;
		XrSwapchainImageAcquireInfo swapchain_acquire_info = {};
		swapchain_acquire_info.type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO;
		result = xrAcquireSwapchainImage(swapchains[i].handle, &swapchain_acquire_info, &swapchain_image_id);
		check_xr_result(result, "Could not acquire swapchain image");

		// We need to wait until the swapchain image is available for writing, as the compositor
		// could still be reading from it (writing while the compositor is still reading could
		// result in tearing or otherwise badly rendered frames).
		// For now, we set the timeout to infinite, but one could also set another timeout
		// by passing in an xrDuration
		XrSwapchainImageWaitInfo swapchain_wait_info = {};
		swapchain_wait_info.type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO;
		swapchain_wait_info.timeout = XR_INFINITE_DURATION;
		result = xrWaitSwapchainImage(swapchains[i].handle, &swapchain_wait_info);
		check_xr_result(result, "Could not wait for the swapchain image");

		// Setup the info we need to render the layer for the current view. The XrCompositionLayerProjectionView
		// is a projection layer element, which has the pose of the current view (pose = location and orientation),
		// the fov of the current view, and the swapchain sub image, which holds the data for the composition
		// layer.
		// The subimage is of type XrSwapchainSubImage, which has a field to the swapchain to display and an
		// imageRect, which represents the valid portion of the image to use (in pixels)
		views[i] = {};
		views[i].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
		views[i].pose = openxr_views[i].pose;
		views[i].fov = openxr_views[i].fov;
		views[i].subImage.swapchain = swapchains[i].handle;
		views[i].subImage.imageRect.offset = { 0, 0 };
		views[i].subImage.imageRect.extent = { swapchains[i].width, swapchains[i].height };

		// Render the content to the swapchain, which is done by the D3D11 handler
		dx11_handler.renderFrame(views[i], swapchains[i].swapchain_data[swapchain_image_id], draw_callback);

		// We're done rendering for the current view, so we can release the swapchain image (i.e. tell
		// the OpenXR runtime that we're done with this swapchain image).
		// We have to pass in a XrSwapchainImageReleaseInfo, but at the moment, this struct doesn't
		// do anything special.
		XrSwapchainImageReleaseInfo swapchain_release_info = {};
		swapchain_release_info.type = XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO;
		result = xrReleaseSwapchainImage(swapchains[i].handle, &swapchain_release_info);
		check_xr_result(result, "Could not release the swapchain image");
	}

	//------------------------------------------------------------------------------------------------------
	// Set the rendered data to be displayed
	//------------------------------------------------------------------------------------------------------
	// Now thta we're done rendering all views, we can update the layer projection we got passed into
	// the method with the rendered views, such that we can display them.
	layer_projection.space = openxr_space;
	layer_projection.viewCount = (uint32_t)views.size();
	layer_projection.views = views.data();
}

ID3D11Device* XrOpenXrHandler::get_device() {
  return dx11_handler.getDevice();
};

ID3D11DeviceContext* XrOpenXrHandler::get_device_context() {
  return dx11_handler.getDeviceContext();
};
