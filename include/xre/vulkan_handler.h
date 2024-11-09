#pragma once

// Vulkan includes
#include <vulkan/vulkan.h>

// OpenXR includes
#define XR_USE_GRAPHICS_API_VULKAN
#include <open_xr/openxr.h>
#include <open_xr/openxr_platform.h>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>

// Other includes
#include <vector>
#include <cstring>
#include <array>
#include <string>
#include <set>

class VulkanHandler {
public:
  VulkanHandler();
  VulkanHandler(XrInstance xr_instance, XrSystemId xr_system_id);

  VkInstance getInstance();
  VkPhysicalDevice getPhysicalDevice();
  VkDevice getLogicalDevice();

private:
  // -------------------------------------------
  // Methods
  // -------------------------------------------
  void initializeVulkan(XrInstance xr_instance, XrSystemId xr_system_id);
  void createInstance();
  void setupDebugMessenger();
  bool checkValidationLayerSupport();
  void setupDevice(XrInstance xr_instance, XrSystemId xr_system_id);


  // -------------------------------------------
  // Attributes
  // -------------------------------------------
  // Vulkan instance
  VkInstance m_vk_instance;

  // Debug messenger (which only prints message that have a certain level)
  VkDebugUtilsMessengerEXT m_debug_messenger;

  // The physical device we'll be using
  VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;

  // Logical device to use
  VkDevice m_device;

  // Graphics queue used for rendering
  VkQueue m_graphics_queue;

  // Queue used for presenting the rendered images
  VkQueue m_present_queue;

  // Whether to enable or disable validation layers
  #ifdef NULL
  const static bool s_enable_validation_layers = false;
  #else
  const static bool s_enable_validation_layers = true;
  #endif
};
