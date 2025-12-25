#include <xre/material.h>

Material::Material() {}

Material::Material(const std::string& vert_path, const std::string& frag_path) {
  m_graphics_pipeline = s_vulkan_handler.createGraphicsPipeline(vert_path, frag_path);
}

void Material::registerVulkanHandler(VulkanHandler handler) {
  s_vulkan_handler = handler;
}

void Material::bind() {
  s_vulkan_handler.bindGraphicsPipeline(m_graphics_pipeline);
}
