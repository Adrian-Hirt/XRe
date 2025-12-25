#include <xre/material.h>

Material::Material(const std::string& vert_path, const std::string& frag_path) {
  m_pipeline_layout = s_vulkan_handler.createPipelineLayout();
  m_graphics_pipeline = s_vulkan_handler.createGraphicsPipeline(m_pipeline_layout, vert_path, frag_path);
}

void Material::registerVulkanHandler(VulkanHandler handler) {
  s_vulkan_handler = handler;
}
