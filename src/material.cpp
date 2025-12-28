#include <xre/material.h>

Material::Material(const std::string& vert_path, const std::string& frag_path, std::shared_ptr<VulkanHandler> vulkan_handler) {
  // Bind the vulkan handler
  m_vulkan_handler = vulkan_handler;

  // Create graphics pipeline
  m_graphics_pipeline = m_vulkan_handler->createGraphicsPipeline(vert_path, frag_path);

  // Create uniform buffer
  m_uniform_buffer = m_vulkan_handler->createUniformBuffer();

  // Create descriptor set
  m_descriptor_set = m_vulkan_handler->allocateDescriptorSet(m_uniform_buffer, NULL, NULL);
}

Material::Material(const std::string& vert_path, const std::string& frag_path, std::shared_ptr<Texture> texture, std::shared_ptr<VulkanHandler> vulkan_handler) {
  // Bind the vulkan handler
  m_vulkan_handler = vulkan_handler;

  // Create graphics pipeline
  m_graphics_pipeline = m_vulkan_handler->createGraphicsPipeline(vert_path, frag_path);

  // Create uniform buffer
  m_uniform_buffer = m_vulkan_handler->createUniformBuffer();

  // Create descriptor set
  m_descriptor_set = m_vulkan_handler->allocateDescriptorSet(
    m_uniform_buffer,
    texture->getTextureImageView(),
    texture->getTextureSampler()
  );
}

Buffer* Material::getUniformBuffer() {
  return m_uniform_buffer;
}

VkDescriptorSet Material::getDescriptorset() {
  return m_descriptor_set;
}

void Material::bind() {
  m_vulkan_handler->bindGraphicsPipeline(m_graphics_pipeline);
}
