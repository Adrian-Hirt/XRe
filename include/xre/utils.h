#pragma once

// OpenXR includes
#include <open_xr/openxr.h>

// DirectX includes
#include <d3d11.h>

// Other includes
#include <iostream>
#include <comdef.h>

// small header-only file which contains some utility functions

namespace Utils {
  // Prints a message and then exits the application
  inline void exitWithMessage(const char *error_string){
    std::cout << error_string << std::endl;
    exit(EXIT_FAILURE);
  };

  inline void checkHresult(HRESULT result, const char *error_string){
    if (FAILED(result)) {
      _com_error err(result);
      LPCTSTR error_message = err.ErrorMessage();
      std::cout << error_message << std::endl;
      exitWithMessage(error_string);
    }
  }

  inline void checkXrResult(XrResult result, const char *error_string){
    if (XR_FAILED(result)) {
      std::cout << result << std::endl;
      exitWithMessage(error_string);
    }
  };

  inline void checkBoolResult(boolean result, const char *error_string){
    if (!result) {
      exitWithMessage(error_string);
    }
  };

  inline std::string getFileLocation(const std::string &path) {
    std::string result;
  #if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    result = path;
  #else
    std::wstring package_install_location = Windows::ApplicationModel::Package::Current->InstalledLocation->Path->Data();
    std::filesystem::path filesys_path(path);

    result = wStringToString(package_install_location);
    result += "\\" + filesys_path.filename().string();
  #endif
    return result;
  };

  inline std::string wStringToString(std::wstring w_string) {
    if (w_string.empty()) {
      return std::string();
    }
    size_t string_size_needed = WideCharToMultiByte(CP_UTF8, 0, w_string.c_str(), w_string.length(), NULL, 0, NULL, NULL);
    std::string result(string_size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, w_string.c_str(), -1, &result[0], string_size_needed, NULL, NULL);
    return result;
  };

  inline std::wstring stringToWString(std::string string) {
    if (string.empty()) {
      return std::wstring();
    }
    size_t string_size_needed = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), string.length(), NULL, 0);
    std::wstring result(string_size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, &result[0], string_size_needed);
    return result;
  }
}
