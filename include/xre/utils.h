#pragma once

// OpenXR includes
#include <open_xr/openxr.h>

// Vulkan includes
#include <vulkan/vulkan.h>

#define GLM_ENABLE_EXPERIMENTAL

// GLM
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/quaternion.hpp>

// Other includes
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

// small header-only file which contains some utility functions

namespace Utils {
// Prints a message and then exits the application
inline void exitWithMessage(const char *error_string) {
  std::cout << error_string << std::endl;
  exit(EXIT_FAILURE);
};

inline void checkVkResult(VkResult result, const char *error_string) {
  if (result != VK_SUCCESS) {
    std::cout << result << std::endl;
    exitWithMessage(error_string);
  }
};

inline void checkXrResult(XrResult result, const char *error_string) {
  if (XR_FAILED(result)) {
    std::cout << result << std::endl;
    exitWithMessage(error_string);
  }
};

inline void checkBoolResult(bool result, const char *error_string) {
  if (!result) {
    exitWithMessage(error_string);
  }
};

inline std::vector<char> readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    std::string error_string = "Failed to open file ";
    error_string.append(filename);
    exitWithMessage(error_string.c_str());
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
};

inline std::vector<const char *> splitString(const std::string &string, char delimiter) {
  std::vector<const char *> result;
  std::stringstream stream(string);
  std::string word;

  while (getline(stream, word, ' ')) {
    const size_t len = word.size() + 1u;
    char *str = new char[len];
    memcpy(str, word.c_str(), len);
    result.push_back(str);
  }

  return result;
}

inline glm::vec3 toVec3(const XrVector3f &vec) { return glm::vec3(vec.x, vec.y, vec.z); }

inline glm::quat toQuat(const XrQuaternionf &quat) { return glm::quat(quat.w, quat.x, quat.y, quat.z); }

// inline std::string getFileLocation(const std::string &path) {
//   std::string result;
// #if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
//   result = path;
// #else
//   std::wstring package_install_location = Windows::ApplicationModel::Package::Current->InstalledLocation->Path->Data();
//   std::filesystem::path filesys_path(path);

//   result = wStringToString(package_install_location);
//   result += "\\" + filesys_path.filename().string();
// #endif
//   return result;
// };

// inline std::string wStringToString(std::wstring w_string) {
//   if (w_string.empty()) {
//     return std::string();
//   }
//   size_t string_size_needed = WideCharToMultiByte(CP_UTF8, 0, w_string.c_str(), w_string.length(), NULL, 0, NULL, NULL);
//   std::string result(string_size_needed, 0);
//   WideCharToMultiByte(CP_UTF8, 0, w_string.c_str(), -1, &result[0], string_size_needed, NULL, NULL);
//   return result;
// };

// inline std::wstring stringToWString(std::string string) {
//   if (string.empty()) {
//     return std::wstring();
//   }
//   size_t string_size_needed = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), string.length(), NULL, 0);
//   std::wstring result(string_size_needed, 0);
//   MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, &result[0], string_size_needed);
//   return result;
// }
} // namespace Utils
