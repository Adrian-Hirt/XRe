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


