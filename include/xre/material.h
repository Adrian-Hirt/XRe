#pragma once

// Vulkan includes
#include <vulkan/vulkan.h>

// XRe includes
#include <xre/vulkan_handler.h>

class Material {
public:
  Material();
  Material(const std::string& vert_path, const std::string& frag_path);
  static void registerVulkanHandler(VulkanHandler handler);
  void bind();

private:
  inline static VulkanHandler s_vulkan_handler;

  VkPipeline m_graphics_pipeline;
  VkPipelineLayout m_pipeline_layout;
};
