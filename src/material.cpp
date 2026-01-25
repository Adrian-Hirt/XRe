#include <xre/material.h>

Material::Material(const std::string &vert_path, const std::string &frag_path, bool persist_between_scenes, std::shared_ptr<VulkanHandler> vulkan_handler) : Material(vert_path, frag_path, NULL, NULL, persist_between_scenes, vulkan_handler) {};

Material::Material(const std::string &vert_path, const std::string &frag_path, std::shared_ptr<Texture> texture,
                   bool persist_between_scenes, std::shared_ptr<VulkanHandler> vulkan_handler) : Material(vert_path, frag_path, texture->getTextureImageView(), texture->getTextureSampler(), persist_between_scenes, vulkan_handler) {};


Material::Material(const std::string &vert_path, const std::string &frag_path, VkImageView textureImageView, VkSampler textureImageSampler,
                   bool persist_between_scenes, std::shared_ptr<VulkanHandler> vulkan_handler) {
  // Bind the vulkan handler
  m_vulkan_handler = vulkan_handler;

  // Create graphics pipeline
  m_graphics_pipeline = m_vulkan_handler->createGraphicsPipeline(vert_path, frag_path);

  // Create uniform buffer
  m_uniform_buffer = std::make_unique<Buffer>(
    m_vulkan_handler->getLogicalDevice(),
    m_vulkan_handler->getPhysicalDevice(), 
    m_vulkan_handler->getAlignedSize() * MAX_MODELS_PER_MATERIAL,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
  );

  // Create descriptor set
  m_descriptor_set = m_vulkan_handler->allocateDescriptorSet(m_uniform_buffer, textureImageView, textureImageSampler, persist_between_scenes);
}

Buffer *Material::getUniformBuffer() { return m_uniform_buffer.get(); }

VkDescriptorSet Material::getDescriptorset() { return m_descriptor_set; }

void Material::bind() { m_vulkan_handler->bindGraphicsPipeline(m_graphics_pipeline); }

uint32_t Material::getNextModelIndex() {
  // Check that the current index would still be valid
  if (m_current_model_index >= MAX_MODELS_PER_MATERIAL) {
    Utils::exitWithMessage("Too many models on this material, max allowed is " + MAX_MODELS_PER_MATERIAL);
  }

  // Returns the current value and increases it
  return m_current_model_index++;
}
