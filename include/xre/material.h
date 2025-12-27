#pragma once

// Vulkan includes
#include <vulkan/vulkan.h>

// XRe includes
#include <xre/vulkan_handler.h>
#include <xre/buffer.h>
#include <xre/texture.h>

class Material {
public:
  Material();
  Material(const std::string& vert_path, const std::string& frag_path);
  Material(const std::string& vert_path, const std::string& frag_path, Texture texture);
  // TODO: replace this by a more sensible architecture
  static void registerVulkanHandler(VulkanHandler handler);
  void bind();

  Buffer* getUniformBuffer();
  VkDescriptorSet getDescriptorset();

private:
  inline static VulkanHandler s_vulkan_handler;

  VkPipeline m_graphics_pipeline;

  // Uniform buffer
  Buffer *m_uniform_buffer = nullptr;

  // Descriptor set
  VkDescriptorSet m_descriptor_set = nullptr;

  Texture *m_texture = nullptr;
};
