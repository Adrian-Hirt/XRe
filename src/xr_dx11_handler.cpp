#include <xre/xre_dx11_handler.h>

//------------------------------------------------------------------------------------------------------
// Empty default constructor which we need
//------------------------------------------------------------------------------------------------------
XreDx11Handler::XreDx11Handler() {};

//------------------------------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------------------------------
XreDx11Handler::XreDx11Handler(LUID &adapter_luid) {
  // Initialize the D3D11 device
  bool device_initialized = initialize_device(adapter_luid);
  check_bool_result(device_initialized, "Could not initialize the D3D11 device!");
};

//------------------------------------------------------------------------------------------------------
// Default destructor
//------------------------------------------------------------------------------------------------------
XreDx11Handler::~XreDx11Handler() {
  if(device != nullptr) {
    device->Release();
    device = nullptr;
  }

  if(device_context != nullptr) {
    device_context->Release();
    device_context = nullptr;
  }
};

//------------------------------------------------------------------------------------------------------
// Initialize the D3D11 device
//------------------------------------------------------------------------------------------------------
bool XreDx11Handler::initialize_device(LUID &adapter_luid) {
  HRESULT result;
  IDXGIFactory *dxgi_factory;

  // Create the DXGI factory we're using to find the correct adapter
  result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)(&dxgi_factory));
  if(FAILED(result)) {
    return false;
  }
  IDXGIAdapter *adapter = nullptr;

  // Loop over all the adapters the factory finds
  for(size_t i = 0; dxgi_factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++) {
    DXGI_ADAPTER_DESC adapter_desc;
    adapter->GetDesc(&adapter_desc);

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
    return false;
  }

  // Set the feature level we wish to use to directx 11
  D3D_FEATURE_LEVEL feature_levels[] = {D3D_FEATURE_LEVEL_11_0};

  // Create the D3D11 device, but don't create any swapchains yet, we'll do that later
  result = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, 0, 0, feature_levels, _countof(feature_levels), D3D11_SDK_VERSION, &device, nullptr, &device_context);

  // Check that we were successful
  if(FAILED(result)) {
    return false;
  }

  // Releae the adapter
  adapter->Release();

  // And we're done
  return true;
}

//------------------------------------------------------------------------------------------------------
// Returns the DirectX Device
//------------------------------------------------------------------------------------------------------
ID3D11Device* XreDx11Handler::get_device() {
  if (this->device == nullptr) {
    exit_with_message("You need to initialize the XreDx11Handler with a LUID to be able to access the device");
    return NULL;
  }
  else {
    return this->device;
  }
};

//------------------------------------------------------------------------------------------------------
// Returns the DirectX DeviceContext
//------------------------------------------------------------------------------------------------------
ID3D11DeviceContext *XreDx11Handler::get_device_context() {
  if (this->device_context == nullptr) {
    exit_with_message("You need to initialize the XreDx11Handler with a LUID to be able to access the device");
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
swapchain_data_t XreDx11Handler::create_render_targets(ID3D11Texture2D &texture) {
  swapchain_data_t result = {};

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
  device->CreateRenderTargetView(&texture, &render_target_view_desc, &result.back_buffer);

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
  device->CreateTexture2D(&depth_buffer_desc, NULL, &depth_buffer);

  // Then use that depth buffer texture object to finally create the depth buffer itself
  D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
  depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT; // Data format for the depth buffer itself is float
  device->CreateDepthStencilView(depth_buffer, &depth_stencil_view_desc, &result.depth_buffer);

  // We don't need the ID3D11Texture2D object anymore. As it's a COM object, it should be freed by calling
  // Release() on it
  depth_buffer->Release();

  return result;
}
