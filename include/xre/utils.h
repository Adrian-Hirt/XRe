#pragma once

// OpenXR includes
#include <open_xr/openxr.h>

// DirectX includes
#include <d3d11.h>

// Other includes
#include <iostream>

// small header-only file which contains some utility functions

// Prints a message and then exits the application
inline void exit_with_message(const char *errorString){
  std::cout << errorString << std::endl;
  exit(EXIT_FAILURE);
};

inline void check_hresult(HRESULT result, const char *errorString){
  if (FAILED(result)) {
    exit_with_message(errorString);
  }
}

inline void check_xr_result(XrResult result, const char *errorString){
  if (XR_FAILED(result)) {
    exit_with_message(errorString);
  }
};

inline void check_bool_result(boolean result, const char *errorString){
  if (!result) {
    exit_with_message(errorString);
  }
};
