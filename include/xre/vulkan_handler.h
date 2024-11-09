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
  void initializeVulkanAndDevices(XrInstance xr_instance, XrSystemId xr_system_id);
  void createRenderPass(VkFormat swapchain_format);
  void createDescriptorSetLayout();
  VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
  VkImageView createDepthImage(VkFormat format, uint32_t width, uint32_t height);

  VkInstance getInstance();
  VkPhysicalDevice getPhysicalDevice();
  VkDevice getLogicalDevice();

private:
  // -------------------------------------------
  // Methods
  // -------------------------------------------
  void createInstance();
  void setupDebugMessenger();
  bool checkValidationLayerSupport();
  void setupDevice(XrInstance xr_instance, XrSystemId xr_system_id);
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
  void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                   VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);


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

  // Swapchain
  VkFormat m_swapchain_format;

  // Render pass for the graphics pipeline
  VkRenderPass m_render_pass;

  // Specifies the types of resources that are going to be accessed by the pipeline
  VkDescriptorSetLayout m_descriptor_set_layout;

  // Whether to enable or disable validation layers
  #ifdef NULL
  const static bool s_enable_validation_layers = false;
  #else
  const static bool s_enable_validation_layers = true;
  #endif
};
