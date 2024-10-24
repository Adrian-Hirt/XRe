#pragma once

// Vulkan includes
#include <vulkan/vulkan.h>

// XRe includes
#include <xre/utils.h>

// Other includes
#include <vector>
#include <cstring>
#include <array>
#include <string>

class VulkanHandler {
public:
  VulkanHandler();

private:
  // -------------------------------------------
  // Methods
  // -------------------------------------------
  void createInstance();
  void setupDebugMessenger();
  bool checkValidationLayerSupport();


  // -------------------------------------------
  // Attributes
  // -------------------------------------------
  // Vulkan instance
  VkInstance m_vk_instance;

  // Debug messenger (which only prints message that have a certain level)
  VkDebugUtilsMessengerEXT m_debug_messenger;

  // Whether to enable or disable validation layers
  #ifdef NDEBUG
  const static bool s_enable_validation_layers = false;
  #else
  const static bool s_enable_validation_layers = true;
  #endif
};
