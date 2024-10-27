#pragma once

// Vulkan includes
#include <vulkan/vulkan.h>

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

private:
  // -------------------------------------------
  // Methods
  // -------------------------------------------
  void initializeVulkan();
  void createInstance();
  void setupDebugMessenger();
  bool checkValidationLayerSupport();
  void setupDevice();


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
  #ifdef NDEBUG
  const static bool s_enable_validation_layers = false;
  #else
  const static bool s_enable_validation_layers = true;
  #endif
};
