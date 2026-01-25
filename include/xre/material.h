#pragma once

// Vulkan includes
#include <vulkan/vulkan.h>

// XRe includes
#include <xre/vulkan_handler.h>
#include <xre/buffer.h>
#include <xre/texture.h>

// Other includes
#include <memory>

class Material {
public:
  Material(const std::string &vert_path, const std::string &frag_path, bool persist_between_scenes, std::shared_ptr<VulkanHandler> vulkan_handler);
  Material(const std::string &vert_path, const std::string &frag_path, std::shared_ptr<Texture> texture,
           bool persist_between_scenes, std::shared_ptr<VulkanHandler> vulkan_handler);

  void bind();
  Buffer *getUniformBuffer();
  VkDescriptorSet getDescriptorset();
  uint32_t getNextModelIndex();

private:
  static constexpr uint32_t MAX_MODELS_PER_MATERIAL = 256;

  // Keep track of the next index of the models into the buffer
  uint32_t m_current_model_index = 0;

  std::shared_ptr<VulkanHandler> m_vulkan_handler;

  VkPipeline m_graphics_pipeline;

  // Uniform buffer
  std::unique_ptr<Buffer> m_uniform_buffer = nullptr;

  // Descriptor set
  VkDescriptorSet m_descriptor_set = nullptr;

  // Optional texture
  std::shared_ptr<Texture> m_texture = nullptr;
};
