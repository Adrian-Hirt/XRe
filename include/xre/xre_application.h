#pragma comment(lib,"D3D11.lib")
#pragma comment(lib,"Dxgi.lib") // for CreateDXGIFactory1
#pragma comment(lib,"D3dcompiler.lib") // To be able to compile the shaders

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11

// DirectX includes
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

// OpenXR includes
#include <open_xr/openxr.h>
#include <open_xr/openxr_platform.h>

// Other includes
#include <iostream>
#include <vector>

class XReApplication {
public:
  XReApplication();
  ~XReApplication();
};
