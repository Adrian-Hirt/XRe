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

private:
  std::shared_ptr<VulkanHandler> m_vulkan_handler;

  VkPipeline m_graphics_pipeline;

  // Uniform buffer
  Buffer *m_uniform_buffer = nullptr;

  // Descriptor set
  VkDescriptorSet m_descriptor_set = nullptr;

  // Optional texture
  std::shared_ptr<Texture> m_texture = nullptr;
};
