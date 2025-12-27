#include <xre/material.h>

Material::Material() {}

Material::Material(const std::string& vert_path, const std::string& frag_path) {
  m_graphics_pipeline = s_vulkan_handler.createGraphicsPipeline(vert_path, frag_path);

  // Create uniform buffer
  m_uniform_buffer = s_vulkan_handler.createUniformBuffer();

  // Create descriptor set
  m_descriptor_set = s_vulkan_handler.allocateDescriptorSet(m_uniform_buffer, NULL, NULL);
}

Material::Material(const std::string& vert_path, const std::string& frag_path, Texture texture) {
  m_graphics_pipeline = s_vulkan_handler.createGraphicsPipeline(vert_path, frag_path);

  // Create uniform buffer
  m_uniform_buffer = s_vulkan_handler.createUniformBuffer();

  // Create descriptor set
  m_descriptor_set = s_vulkan_handler.allocateDescriptorSet(
    m_uniform_buffer,
    texture.getTextureImageView(),
    texture.getTextureSampler()
  );
}

Buffer* Material::getUniformBuffer() {
  return m_uniform_buffer;
}

VkDescriptorSet Material::getDescriptorset() {
  return m_descriptor_set;
}

void Material::registerVulkanHandler(VulkanHandler handler) {
  s_vulkan_handler = handler;
}

void Material::bind() {
  s_vulkan_handler.bindGraphicsPipeline(m_graphics_pipeline);
}
