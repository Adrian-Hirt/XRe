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

