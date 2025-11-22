#pragma once

// XRe includes
#include <xre/utils.h>

// Vulkan includes
#include <vulkan/vulkan.h>

namespace VulkanUtils {
  inline static uint32_t findMemoryType(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
      bool supported = type_filter & (1 << i);
      bool flags_match = (memory_properties.memoryTypes[i].propertyFlags & properties) == properties;
      if (supported && flags_match) {
        return i;
      }
    }

    Utils::exitWithMessage("Failed to find suitable memory type!");
    return 0;
  }
}
