#include <xre/dx11_handler.h>

//------------------------------------------------------------------------------------------------------
// Empty default constructor which we need
//------------------------------------------------------------------------------------------------------
Dx11Handler::Dx11Handler() {};

//------------------------------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------------------------------
Dx11Handler::Dx11Handler(LUID &adapter_luid) {
  // Initialize the D3D11 device
  initializeDevice(adapter_luid);

  // Initialize the device states
  initializeDeviceStates();
};

//------------------------------------------------------------------------------------------------------
// Initialize the D3D11 device
//------------------------------------------------------------------------------------------------------
void Dx11Handler::initializeDevice(LUID &adapter_luid) {
  HRESULT result;
  IDXGIFactory1 *dxgi_factory;

  // Create the DXGI factory we're using to find the correct adapter
  result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)(&dxgi_factory));
  Utils::checkHresult(result, "Could not create the DXGI factory!");

  IDXGIAdapter1 *adapter = nullptr;

  // Loop over all the adapters the factory finds
  for(size_t i = 0; dxgi_factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++) {
    DXGI_ADAPTER_DESC1 adapter_desc;
    adapter->GetDesc1(&adapter_desc);

    // check wether the LUID of the current adapter matches the one we're looking for
    if (memcmp(&adapter_desc.AdapterLuid, &adapter_luid, sizeof(&adapter_luid)) == 0) {
      break;
    }
    else {
      adapter->Release();
      adapter = nullptr;
    }
  }

  // Release the factory
  dxgi_factory->Release();

  // If we did not find an adapter, we can exit the function
  if(adapter == nullptr) {
    Utils::exitWithMessage("Did not find an adapter to create the device with!");
  }

  // Set the feature level we wish to use to directx 11
  D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };

  // Create the D3D11 device, but don't create any swapchains yet, we'll do that later
  result = D3D11CreateDevice(
    adapter,
    D3D_DRIVER_TYPE_UNKNOWN,
    0,
    0,
    feature_levels,
    _countof(feature_levels),
    D3D11_SDK_VERSION,
    &device,
    nullptr,
    &device_context);

  // Check that we were successful
  Utils::checkHresult(result, "Creating the device and device_context failed");

  // Releae the adapter
  adapter->Release();

  // And we're done
}

void Dx11Handler::initializeDeviceStates() {
  HRESULT result;

  // First, build the default device state for the rasterizer,
  // where we cull the back and fill the triangles with solid color.
  // Also, we use antialiazing for lines.
  D3D11_RASTERIZER_DESC rasterizer_description;
  ZeroMemory(&rasterizer_description, sizeof(rasterizer_description));
  rasterizer_description.FillMode = D3D11_FILL_SOLID;
  rasterizer_description.CullMode = D3D11_CULL_BACK;
  rasterizer_description.AntialiasedLineEnable = true;
  rasterizer_description.FrontCounterClockwise = false;

  result = device->CreateRasterizerState(&rasterizer_description, &p_rasterizer_state_default_cw);
  Utils::checkHresult(result, "Failed to create the rasterizer state for default CW mode");

  // Build the same state again, but this time use counter-clockwise
  // culling. This is needed to draw transparent objects twice, such
  // that the backside of the object also appears in the view
  ZeroMemory(&rasterizer_description, sizeof(rasterizer_description));
  rasterizer_description.FillMode = D3D11_FILL_SOLID;
  rasterizer_description.CullMode = D3D11_CULL_BACK;
  rasterizer_description.AntialiasedLineEnable = true;
  rasterizer_description.FrontCounterClockwise = true;

  result = device->CreateRasterizerState(&rasterizer_description, &p_rasterizer_state_default_ccw);
  Utils::checkHresult(result, "Failed to create the rasterizer state for default CW mode");

  // Then build the wireframe device state for the rasterizer,
  // where we cull the back, only render the wireframe lines,
  // disable multisampling but use antialiasing on the lines
  ZeroMemory(&rasterizer_description, sizeof(rasterizer_description));
  rasterizer_description.FillMode = D3D11_FILL_WIREFRAME;
  rasterizer_description.CullMode = D3D11_CULL_BACK;
  rasterizer_description.AntialiasedLineEnable = true;

  result = device->CreateRasterizerState(&rasterizer_description, &p_rasterizer_state_wireframe);
  Utils::checkHresult(result, "Failed to create the rasterizer state for wireframe mode");

  // Use the default state for now
  device_context->RSSetState(p_rasterizer_state_default_cw);

  // Next, we setup the texture sampler state
  D3D11_SAMPLER_DESC sampler_description;
  ZeroMemory(&sampler_description, sizeof(sampler_description));
  sampler_description.Filter = D3D11_FILTER_ANISOTROPIC; // Use anisotropic filtering
  sampler_description.MaxAnisotropy = 8; // And set anisotropic filter to x8
  // Repeat the texture in U, V and W directions
  sampler_description.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  sampler_description.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sampler_description.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sampler_description.MinLOD = 0; // Use level 0 (normal texture) as the min MipMap level
  sampler_description.MaxLOD = FLT_MAX; // And allow any arbitrary high MipMap level

  result = device->CreateSamplerState(&sampler_description, &p_sampler_state);
  Utils::checkHresult(result, "Failed to create the sampler state");
  device_context->PSSetSamplers(0, 1, &p_sampler_state);

  // Next, we setup the blend state
  D3D11_BLEND_DESC blend_description;
  ZeroMemory(&blend_description, sizeof(blend_description));
  blend_description.RenderTarget[0].BlendEnable = true; // Enable blending (alpha values)
  blend_description.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // Add destination color to the source color
  blend_description.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // Use the alpha value of the source to set its value
  blend_description.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // And use 1-src_alpha for the dest alpha
  blend_description.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; // Use default method of add
  blend_description.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE; // Use the full source alpha
  blend_description.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO; // And use no destination alpha
  blend_description.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // Draw RGBA values
  blend_description.IndependentBlendEnable = true; // Only use the first render target
  blend_description.AlphaToCoverageEnable = false; // Improve quality of transparent textures

  result = device->CreateBlendState(&blend_description, &p_blend_state);
  Utils::checkHresult(result, "Failed to create the blend state");
  device_context->OMSetBlendState(p_blend_state, 0, 0xffffffff);
}

//------------------------------------------------------------------------------------------------------
// Returns the DirectX Device
//------------------------------------------------------------------------------------------------------
ID3D11Device* Dx11Handler::getDevice() {
  if (this->device == nullptr) {
    Utils::exitWithMessage("You need to initialize the Dx11Handler with a LUID to be able to access the device");
    return NULL;
  }
  else {
    return this->device;
  }
};

//------------------------------------------------------------------------------------------------------
// Returns the DirectX DeviceContext
//------------------------------------------------------------------------------------------------------
ID3D11DeviceContext *Dx11Handler::getDeviceContext() {
  if (this->device_context == nullptr) {
    Utils::exitWithMessage("You need to initialize the Dx11Handler with a LUID to be able to access the device");
    return NULL;
  }
  else {
    return this->device_context;
  }
};

//------------------------------------------------------------------------------------------------------
// Create the targets for DirectX to render to. This method creates a back-buffer (which stores the
// image information) as well as the matching z-buffer (which stores the depth-information of the
// scene to render)
//------------------------------------------------------------------------------------------------------
swapchain_data_t Dx11Handler::createRenderTargets(ID3D11Texture2D &texture) {
  swapchain_data_t result_target = {};
  HRESULT result;

  //----------------------------------------------------------------------------------
  // Create the backbuffer
  //----------------------------------------------------------------------------------
  // We need to pass some aditional data to the CreateRenderTargetView function, so we create
  // a Render target view desc and add the format of the swapchain, as well as the view dimension
  // to that description struct.
  // The swapchain image that OpenXR created uses a TYPELESS format, however, we need a "typed"
  // format. As such, we use the DXGI_FORMAT_R8G8B8A8_UNORM format, which is "A four-component, 32-bit
  // unsigned-normalized-integer format that supports 8 bits per channel including alpha" (according
  // to MSDN. That way, we can specify the colors with RGBA values between 0 and 255.
  D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc = {};
  render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
  render_target_view_desc.Format = d3d11_swapchain_format;
  result = device->CreateRenderTargetView(&texture, &render_target_view_desc, &result_target.back_buffer);
  Utils::checkHresult(result, "Could not create the render target view");

  //----------------------------------------------------------------------------------
  // Create a matching depth buffer (z-buffer)
  //----------------------------------------------------------------------------------
  // As we can't directly use the .texture field of the swapchain image as for the backbuffer,
  // we first need to fetch information about the swapchain image (ID3D11Texture2D) that OpenXR
  // created and then we can use that to information to manually construct a texture object
  D3D11_TEXTURE2D_DESC image_desc = {};
  texture.GetDesc(&image_desc);

  // Create the depth buffer description
  D3D11_TEXTURE2D_DESC depth_buffer_desc = {};
  depth_buffer_desc.SampleDesc.Count = 1;       // Do not use supersampling for now
  depth_buffer_desc.MipLevels = 1;              // Multiple mipmap levels are only useful for textures, for backbuffer only need one level
  depth_buffer_desc.Width = image_desc.Width;   // Use same width as the swapchain image that OpenXR created
  depth_buffer_desc.Height = image_desc.Height; // Use same height as the swapchain image that OpenXR created
  depth_buffer_desc.ArraySize = image_desc.ArraySize;
  depth_buffer_desc.Format = DXGI_FORMAT_R32_TYPELESS; // Use TYPELESS format, such that we have the same as for the image that OpenXR created
  depth_buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;

  // Create the depth buffer texture object
  ID3D11Texture2D *depth_buffer;
  result = device->CreateTexture2D(&depth_buffer_desc, NULL, &depth_buffer);
  Utils::checkHresult(result, "Could not create the texture 2d");

  // Then use that depth buffer texture object to finally create the depth buffer itself
  D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
  depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT; // Data format for the depth buffer itself is float
  result = device->CreateDepthStencilView(depth_buffer, &depth_stencil_view_desc, &result_target.depth_buffer);
  Utils::checkHresult(result, "Could not create the depth stencil view");

  // We don't need the ID3D11Texture2D object anymore. As it's a COM object, it should be freed by calling
  // Release() on it
  depth_buffer->Release();

  return result_target;
}


//------------------------------------------------------------------------------------------------------
// Render a frame
//------------------------------------------------------------------------------------------------------
void Dx11Handler::renderFrame(XrCompositionLayerProjectionView& view,
                                  swapchain_data_t& swapchain_data,
                                  std::function<void(XrCompositionLayerProjectionView&)> draw_callback) {
  //----------------------------------------------------------------------------------
	// Setup viewport
	//----------------------------------------------------------------------------------
	// For D3D11 to render correctly, we need to create a D3D11_VIEWPORT struct and
	// set the top left XY coordinates of the viewport, as well as the width and height
	// of the viewport.
	// As this should match the size of the swapchain, we just use the size of the
	// subimage we set previously
	XrRect2Di& image_rect = view.subImage.imageRect;
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = (float)image_rect.offset.x;
  viewport.TopLeftY = (float)image_rect.offset.y;
  viewport.Width = (float)image_rect.extent.width;
  viewport.Height = (float)image_rect.extent.height;
  viewport.MinDepth = 0;
  viewport.MaxDepth = 1;

	// Now we can set the viewport of the device context
	device_context->RSSetViewports(1, &viewport);

	//----------------------------------------------------------------------------------
	// Clear the Buffers
	//----------------------------------------------------------------------------------
	// It's usually nessecary to clear the backbuffer (as it usually still contains the
	// data from the previous frame). This is usually done by setting all the data
	// (pixels) to a single color.
	float clear_color[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	device_context->ClearRenderTargetView(swapchain_data.back_buffer, clear_color);

	// Also clear the depth buffer, such that it's ready for rendering
	device_context->ClearDepthStencilView(swapchain_data.depth_buffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//----------------------------------------------------------------------------------
	// Set the render target
	//----------------------------------------------------------------------------------
	// Now we can set the target of all render operations to the backbuffer of the
	// swapchain we're using.
	// This will render all our content to that backbuffer.
	device_context->OMSetRenderTargets(1, &swapchain_data.back_buffer, swapchain_data.depth_buffer);

  //----------------------------------------------------------------------------------
	// Update the shader constant buffers
	//----------------------------------------------------------------------------------
  // Compute the viewProjectionMatrix as well as the model matrix
  // and send them to the GPU
  Shader::updateViewProjectionMatrix(Geometry::computeViewProjectionMatrix(view));

  //----------------------------------------------------------------------------------
	// Draw the scene
	//----------------------------------------------------------------------------------
  draw_callback(view);
}

void Dx11Handler::useDefaultRasterizer(bool use_clockwise) {
  if (use_clockwise) {
    device_context->RSSetState(p_rasterizer_state_default_cw);
  }
  else {
    device_context->RSSetState(p_rasterizer_state_default_ccw);
  }
};

void Dx11Handler::useWireframeRasterizer() {
  device_context->RSSetState(p_rasterizer_state_wireframe);
};
